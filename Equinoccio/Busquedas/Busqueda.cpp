#include "Busqueda.h"
#include "../Busqueda Binaria/Buscador.h"
#include "../Registros/Registro.h"
#include "../Registros/RegistroNGramas.h"
#include "../Parsers/Parser.h"
#include "../FileManager/ConstPath.h"
#include "../FileManager/FileManager.h"
#include "../Busqueda Rankeada/BusquedaRankeada.h"

Busqueda::Busqueda() {}

Busqueda::~Busqueda() {
}

std::list<std::string>* Busqueda::buscar(std::string& consulta, std::string catalogo, bool rankeada) {
	std::list<std::string> *paths= new std::list<std::string>;
	if (consulta.size() != 0) {
		uint32_t segmentos= FileManager::getCantidadSegmentos();
		if (consulta.find('*') != std::string::npos || consulta.find('?') != std::string::npos )
			rankeada=false;
		if(!rankeada) {
			size_t pos = 0;
			size_t where = 0;
			bool encontrado;
			for(uint32_t seg=0; seg<segmentos; seg++) {
				punteros.clear();
				punteros_match.clear();
				do {
					//tomo la palabra y la busco en el indice
					where = consulta.find(' ', pos);
					encontrado = buscarEnIndice(consulta.substr(pos, where-pos), catalogo, seg);
					pos = where+1;
				}while (where != std::string::npos && encontrado);

				if (!encontrado) {
					//una o mas palabras no matcheadas
					std::cout<<" * NO MATCH SEGMENTO " << seg << " * " <<std::endl;
					for (unsigned int i=0; i<punteros.size();i++)
						delete punteros[i];
				}
				else {
					 std::cout<<" * AND SEGMENTO " << seg << " * " <<std::endl;
					 Busqueda::andPunteros2(this->punteros,this->punteros_match);

					 //agregar los paths a la lista
					 if (punteros_match.size() != 0) {
					  do{
						   std::string path_documento = buscarPath(punteros_match.front(), catalogo, seg);
						   std::cout<<"MATCH: "<<path_documento<<std::endl;
						   paths->push_back(path_documento);
						   punteros_match.pop_front();
					  }while(punteros_match.size()>0);
					 }
				}
			}// for segmentos
		}
		else {
			std::list<BusquedaRankeada::RegConsulta*> arbol;
			RedBlackTree<RegRank> arbol_segm;
			BusquedaRankeada::RegConsulta *res;
			RegRank *reg;
			RegRank comp_reg(0,0,0);
			for (uint32_t segm=0; segm<segmentos; segm++) {
				if (BusquedaRankeada::coseno(consulta,catalogo,arbol, segm)) {
					while (!arbol.empty()) {
						res = arbol.front();
						arbol.pop_front();
						reg = new RegRank(res->nro, segm, res->peso);
						arbol_segm.Insert(reg);
						delete res;
					}
				}
			}
			//busco los path de los documentos match
			while ((reg = arbol_segm.RemoverMayorIgual(comp_reg))) {
				std::string path_doc = buscarPath(reg->nro,catalogo,reg->segm);
				std::cout<<"MATCH "<<"peso: "<<1-reg->peso<<" "<<path_doc<<std::endl;
				paths->push_back(path_doc);
				delete reg;
			}
		}
	}
	std::cout<<"Fin buscar"<<std::endl;
	return paths;
}
	
bool Busqueda::buscarEnIndice(std::string consulta, std::string catalogo, uint32_t segmento) {

	if ( consulta.find('*')==std::string::npos && consulta.find('?')==std::string::npos) {
		//busqueda simple
		Util::aMinusculas(consulta);
		std::cout<<"Busqueda simple: \""<<consulta<<"\""<<std::endl;
		if (consulta.size() != 0) {
			RegistroIndice reg = Buscador::buscar(consulta, catalogo, segmento);
			if ( reg.frec != 0) {
				//obtener los punteros
				std::string nombre_pun =  FileManager::obtenerPathBase(segmento);
				nombre_pun += catalogo;
				nombre_pun += ".pun";
				std::cout<<nombre_pun<<std::endl;
				std::ifstream arch_punteros(nombre_pun.c_str(), std::ios::in | std::ios::binary);
				if (arch_punteros.good()){
					std::list<uint32_t>* puntDocs = new std::list<uint32_t>;
					Registro::obtenerPunterosEnLista(arch_punteros, reg.pDocs , reg.frec, puntDocs);
					punteros.push_back(puntDocs);
					arch_punteros.close();
					return true;
				}
				std::cout<<"error al abrir el arch de punteros: "<<nombre_pun<<std::endl;
				return false;
			}
		}
	}
	else {
		//consulta con comodines
		std::cout<<"busqueda con comodines: "<<consulta<<std::endl;
		return consultaNgramas(consulta, catalogo);
	}
	return false;
}

bool Busqueda::consultaNgramas(std::string& consulta, std::string catalogo) {

     // TODO: deberia buscar en todos los segmentos
	std::string path = FileManager::obtenerPathBase(0);
	path += catalogo;
	path += ".idx";
	std::cout<<path<<std::endl;
	std::ifstream indice(path.c_str(), std::ios::in | std::ios::binary);
	// TODO: IDEM
	path = FileManager::obtenerPathBase(0);
	path += catalogo;
	path += EXT_NG_PUN;
	std::cout<<path<<std::endl;
	std::ifstream pun_ng(path.c_str(),  std::ios::in | std::ios::binary);
	// TODO: IDEMx2
	path = FileManager::obtenerPathBase(0);
	path += catalogo;
	path += ".lex";
	std::cout<<path<<std::endl;
	std::ifstream lexico (path.c_str(), std::ios::in | std::ios::binary);
	// TODO: IDEMx3
	path = FileManager::obtenerPathBase(0);
	path += catalogo;
	path += ".pun";
	std::cout<<path<<std::endl;
	std::ifstream pun_docs(path.c_str(), std::ios::in | std::ios::binary);
	if (!indice.good() || !pun_ng.good() || !lexico.good() || !pun_docs.good()) {
		std::cout << "error al abrir los archivos de ngramas"<<std::endl;
		indice.close(); pun_ng.close();  lexico.close();  pun_docs.close();
		return false;
	}

	std::string str;
	str = '$';
	size_t pos = 0;
	size_t where = 0;
	bool hay_bigrama = false;
	std::list<std::string> substr; 	//lista con los substrings a matchear por los terminos
	std::vector<std::list<uint32_t>* > offset_indice;	//offsets de los terminos de bigramas en el indice general
	std::list<uint32_t> *lista_offset;

	do {
		//separo por asteriscos
		where = consulta.find('*', pos);
		str += consulta.substr(pos, where - pos);
		Util::aMinusculas(str);
		if (where == std::string::npos) {
			str +='$';
		}
		if (str.size() >= 2){
			hay_bigrama = true;
			//armo bigramas y llamo a buscar para cada uno
			substr.push_back(str);
			for (size_t car=0; car<(str.size()-1) ;car++) {
				if (str[car] != '?' && str[car+1]!= '?') {
					RegistroNGrama regN = Buscador::buscarNgrama(str.substr(car,2),catalogo);
					//en pDocs esta el offset al archivo con los offsets al indice general
					if (regN.frec > 0) {
						//busco la lista de offsets al indice asociado a ese biGrama
						lista_offset = new std::list<uint32_t>;
						RegistroNGramas::obtenerPunterosEnLista(pun_ng,regN.pDocs, regN.frec, lista_offset);
						offset_indice.push_back(lista_offset);
					}
					else {
						//liberar listas
						for (unsigned int i=0; i<offset_indice.size();i++)
							delete offset_indice[i];
						indice.close();
						lexico.close();
						pun_ng.close();
						pun_docs.close();
						return false;
					}
				}
			}
		}
		else {
			if (str.size() == 1 && str[0]!='?') {
				substr.push_back(str);
			}
		}
		str.clear();
		pos = where+1;
	}while(where != std::string::npos);

	pun_ng.close();
	if (!hay_bigrama) {
		lexico.close();
		indice.close();
		pun_docs.close();
		return false;
	}

	//Realizo el AND entre los punteros al indice obtenidos
	std::list<uint32_t> offset_and;
	Busqueda::andPunteros2(offset_indice, offset_and);

	RegIndice *r;
	uint32_t off = 0;
	std::list<RegIndice*> registros;
	//obtengo los registros del indice

	while (!offset_and.empty()) {
		r = new RegIndice;
		r->frec = 0;
		r->pun = 0;
		// entro al indice
		//obtengo el termino, la frec, y el puntero a los docs y los agrego a la lista
		indice.seekg(offset_and.front(),std::ios_base::beg);
		//leo el offset al lexico
		indice.read((char*)&off, sizeof(uint32_t));
		//leo frecuencia y puntero
		indice.read((char*)&(r->frec), sizeof(uint32_t));
		indice.read((char*)&(r->pun), sizeof(uint32_t));

		//busco el termino en el archivo de lexico
		lexico.seekg(off,std::ios_base::beg);
		char c=0;
		r->termino.clear();
		while(lexico.good() && (c = lexico.get()) != 0){
		  r->termino += c;
		}
		std::cout<<"Termino encontrado: "<<r->termino<<std::endl;
		registros.push_back(r);
		offset_and.pop_front();
	}

	lexico.close(); //checkear error de lectura
	indice.close();

	//eliminar falsos positivos
	std::list<RegIndice*> reg_match;
	filtrarFalsosPositivos(substr,registros, reg_match);

	std::vector< std::list<uint32_t>* > punteros_docs;
	//obtengo la lista de punteros de cada termino
	std::list<uint32_t> *punt = NULL;
	while (!reg_match.empty()) {
		punt = new std::list<uint32_t>;
		punt->clear();
		Registro::obtenerPunterosEnLista(pun_docs,reg_match.front()->pun,reg_match.front()->frec,punt);
		delete reg_match.front();
		reg_match.pop_front();
		punteros_docs.push_back(punt);
	}
	pun_docs.close();
	//union para evitar docs repetidos
	if ((punt = unionPunteros2(punteros_docs)) != NULL) {
		punteros.push_back(punt);
		return true;
	}
	return false;	//falta verificar que se libere todo
}

void Busqueda::andPunteros(std::vector< std::list<uint32_t>* > &punteros, std::list<uint32_t> &punteros_and) {

    if (punteros.size() > 1) {
	  uint32_t pos_min=0;
	  uint32_t min=(uint32_t)-1;
	  std::vector<uint32_t> vec_min;
	  uint32_t palabrasBuscadas = punteros.size();
	  while(punteros.size()>0){
	       vec_min.clear();
	       min = (uint32_t)-1;
	       for (uint32_t i=0;i<punteros.size();i++){
		    if(min > punteros[i]->front()){
			 vec_min.clear();
			 min = punteros[i]->front();
			 pos_min=i;
			 vec_min.push_back(i);
		    }else if (min==punteros[i]->front()){
			 vec_min.push_back(i);
		    }
	       }
	       if(vec_min.size() == palabrasBuscadas) //es un AND, todas tienen que ser menores
		    punteros_and.push_back(min);
	       for (uint32_t i=0;i<vec_min.size();i++){
		    punteros[vec_min[i]]->pop_front();
	       }
	       for (uint32_t i=0;i<punteros.size();i++){
		    if(punteros[i]->size() == 0){
			 delete punteros[i];
			 punteros[i] = punteros[punteros.size() -1];
			 i--; //debe ser la tercera vez que lo hago en este TP
			 punteros.pop_back();
		    }
	       }
	  }
    }
    else{
	  punteros_and = *punteros[0];
    }
}

void Busqueda::andPunteros2(std::vector< std::list<uint32_t>* > &punteros, std::list<uint32_t> &punteros_and) {

    if (punteros.size() > 1){
	  uint32_t min=(uint32_t)-1;
	  std::vector<std::list<uint32_t>::iterator> vec_it;

	  bool salir=false;

	  // cargo un vector con iteradores al principio de cada lista
	  for(int i=0;i<(int)punteros.size();i++)
	       vec_it.push_back(punteros[i]->begin());

	  min=*vec_it[0];
	  for(int i=0;i<(int)punteros.size() && !salir;i++){
	       while(vec_it[i]!=punteros[i]->end() && *vec_it[i] < min)
		    vec_it[i]++;
	       if(vec_it[i] == punteros[i]->end())
		    salir=true;
	       else if(min < *vec_it[i]){
		    min = *vec_it[i];
		    i=-1;
	       }
	       else if(i == (int)(punteros.size()-1)){
		    punteros_and.push_back(min);
		    vec_it[0]++;
		    if(vec_it[0] !=punteros[0]->end())
			 min = *vec_it[0];
		    else salir = true;
		    i=-1;
	       }
	  }
    }
    else{
	  punteros_and = *punteros[0];
    }

    for(int i=0;i<(int)punteros.size();i++){
	 delete punteros[i];
    }
}


void Busqueda::filtrarFalsosPositivos(std::list<std::string>& consulta, std::list<RegIndice*> &lista, std::list<RegIndice*> &filtrada) {

	if (!consulta.empty()) {
		std::list<std::string>::iterator it_str;
		std::string termino;
		RegIndice* reg;

		//recorro todos los registros verificando los falsos positivos
		while (!lista.empty()){
			reg = lista.front();
			termino = '$';
			termino += reg->termino;
			termino += '$';
			it_str = consulta.begin();

			bool encontrado = true;
			size_t pos_wild, pos=0;
			char c;
			while (encontrado && it_str != consulta.end() && pos<termino.size()) {
				pos_wild=0;
				c=(*it_str)[0];
				if(c!='?') {
					pos = termino.find(c,pos);
				}
				while(encontrado && pos_wild<it_str->size() && pos<termino.size()){
					if ((*it_str)[pos_wild] != '?') {
						if((*it_str)[pos_wild] != termino[pos])
							encontrado=false;
					}
					pos++;
					pos_wild++;
				}
				it_str++;
			}

			if (it_str == consulta.end() && encontrado && pos==termino.size()) {
				//salio porque encontro todos los subterminos, entonces agrego el registro a la lista
				filtrada.push_back(reg);
				std::cout<<"palabra match: "<<reg->termino<< " Frec: "<<reg->frec<<std::endl;
			}
			else
				delete lista.front();
			lista.pop_front();
		}
	}
}

std::list<uint32_t>* Busqueda::unionPunteros(std::vector< std::list<uint32_t>* > &punteros) {

	std::list<uint32_t>* pun_union = NULL;

	if (punteros.size() > 1) {
		pun_union = new std::list<uint32_t>;
		uint32_t anterior = -1; //ver inicial
		do {
			//busco el minimo
			uint32_t min = 0; 	//lista del minimo puntero
			for (uint32_t i=1; i < punteros.size();i++){
				if (punteros[i]->front() <= punteros[min]->front()) {
					min = i;
				}
			}
			if (punteros[min]->front() != anterior) {
				anterior = punteros[min]->front();
				pun_union->push_back(anterior);
			}
			punteros[min]->pop_front();
			//si la lista del minimo esta vacia la borro
			if (punteros[min]->empty()) {
				delete punteros[min];
				punteros[min] = punteros[punteros.size()-1];
				punteros.pop_back();
			}
		}while(!punteros.empty());

		if (pun_union->empty()) {
			delete pun_union;
			pun_union = NULL;
		}
	}
	else {
		if (punteros.size() == 1)
			pun_union = punteros[0]; //tiene un solo elemento
	}
	return pun_union;
}


std::list<uint32_t>* Busqueda::unionPunteros2(std::vector< std::list<uint32_t>* > &punteros) {

     std::list<uint32_t>* pun_union = NULL;

     if (punteros.size() > 1) {
	  std::vector<std::list<uint32_t>::iterator> iteradores;

	  uint32_t min=(uint32_t)-1;
	  uint32_t proximo=(uint32_t)-1;

	  for(int i=0;i<(int)punteros.size();i++){
	       iteradores.push_back(punteros[i]->begin());
	       if(*iteradores[i] < min)
		    min = *iteradores[i];
	  }

	  pun_union = new std::list<uint32_t>;

	  while(iteradores.size()>0){
	       for(int i=0;i<(int)iteradores.size(); i++){
		    while(iteradores[i]!= punteros[i]->end() && *iteradores[i]<=min)
			 iteradores[i]++;

		    if(iteradores[i] == punteros[i]->end()){
			 delete punteros[i];
			 punteros.erase(punteros.begin()+i);
			 iteradores.erase(iteradores.begin()+i);
			 i--;
		    }
		    else if(*iteradores[i] < proximo)
			 proximo = *iteradores[i];
	       }
	       
	       pun_union->push_back(min);
	       min=proximo;
	       proximo=(uint32_t)-1;
	  }
     }else{
    	 if (punteros.size()==1){
			 pun_union = punteros[0];
    	 }
     }
	
     return pun_union;
}

std::string Busqueda::buscarPath(uint32_t puntero,std::string catalogo, uint32_t segmento) {

	std::string path;
	uint32_t par[2]; //par offsetLex - numDir
	// deberia buscar el path en el segmento que corresponda a la busqueda
	std::string nombre = FileManager::obtenerPathIdxArch(segmento);
	nombre +=catalogo;
	std::ifstream indiceDocs(nombre.c_str(),  std::ios::in | std::ios::binary);
	if (indiceDocs.good()) {
		//obtengo el puntero al lexico de archivos
	    indiceDocs.seekg(puntero*(2*sizeof(uint32_t)+sizeof(time_t)+sizeof(ino_t)));
		indiceDocs.read((char*)par, 2*sizeof(uint32_t));
		indiceDocs.close();
		nombre = FileManager::obtenerPathLexArch(segmento);
		nombre += catalogo;
		std::ifstream lexDocs(nombre.c_str(), std::ios::in);
		if(lexDocs.good()) {
			//obtengo el nombre del documento
			lexDocs.seekg(par[0]);
			std::getline(lexDocs, nombre, '\0');
			lexDocs.close();
			//busco el directorio
			lexDocs.open(FileManager::obtenerPathIdxDirs(segmento).c_str(), std::ios::in | std::ios::binary);
			if(lexDocs.good()){
				//obtengo el offset del directorio
				uint32_t offset;
				lexDocs.seekg(4*par[1]);
				lexDocs.read((char*)&offset, sizeof(uint32_t));
				lexDocs.close();
				lexDocs.open(FileManager::obtenerPathLexDirs(segmento).c_str(),  std::ios::in);
				if (lexDocs.good()) {
					lexDocs.seekg(offset);
					std::getline(lexDocs, path, '\0');
					lexDocs.close();
					path += '/';
					path += nombre;
				}
				else
					std::cout<<"error al abrir el lexico de directorios"<<std::endl;
			}
			else {
				std::cout<<"error al abrir el indice de directorios"<<std::endl;
			}
		}
		else {
			std::cout<<"error al abrir el lexico de documentos: "<<nombre<<std::endl;
		}
	}
	else {
		std::cout<<"error al abrir el indice de documentos: "<<nombre<<std::endl;
	}
	return path;
}


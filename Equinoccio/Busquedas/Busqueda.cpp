#include "Busqueda.h"
#include "../Busqueda Binaria/Buscador.h"
#include "../Registros/Registro.h"
#include "../Parsers/Parser.h"
#include "../FileManager/ConstPath.h"

Busqueda::Busqueda() {
	size = 0;
}

Busqueda::~Busqueda() {
	borrarListas();
}

std::list<std::string> Busqueda::buscar(std::string& consulta, std::string catalogo) {

	std::list<std::string> paths;
	if (consulta.size() != 0) {

		size_t pos = 0;
		size_t where = 0;
		bool encontrado;
		do {
			//tomo la palabra y la busco en el indice
			where = consulta.find(' ', pos);
			std::cout<<"Buscar: "<<consulta.substr(pos, where-pos)<<" where: "<<where<<" pos: "<<pos<<std::endl;
			encontrado = buscarEnIndice(consulta.substr(pos, where-pos), catalogo);
			pos = where+1;
		}while (where != std::string::npos && encontrado);

		if (!encontrado) {
			//una o mas palabras no matcheadas
			std::cout<<" * NO MATCH * "<<std::endl;
		}
		else {
		     if (punteros.size() > 1){
			  //hacer un AND de todos los punteros y guardarlos en punteros_match
			  //punteros_match list
			  //punteros es un vector listas...

			  std::cout << punteros.size() << " listas\n";
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
					 std::cout << "minimo: " << min << " posicion " << i<<std::endl;
					 pos_min=i;
					 vec_min.push_back(i);
				    }else if (min==punteros[i]->front()){
					 vec_min.push_back(i);
				    }
			       }
			       if(vec_min.size() == palabrasBuscadas) //es un AND, todas tienen que ser menores
				    punteros_match.push_back(min);
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
			       std::cout << "MATCH: (min) " << min << std::endl;
			  }
		     }
		     else{
			  std::cout << "Se busco una sola palabra, se saltea el AND\n";
			  punteros_match = *punteros[0];
		     }
		     std::cout << "tamanio de la lista final: " << punteros_match.size() << std::endl;
		     std::list<uint32_t>::iterator it;
		     std::list<uint32_t>::iterator end = punteros_match.end();
		     
		     //agregar los paths a la lista
		     for (it = punteros_match.begin(); it != end; it++)
			  paths.push_back(buscarPath(*it, catalogo));
		     
		     while(punteros_match.size()>0){
			  std::cout<<"MATCH: "<<buscarPath(punteros_match.front(), catalogo)<<std::endl;
			  punteros_match.pop_front();
		     }
		}
	}
		
	//borrarListas();
	std::cout<<"Fin buscar"<<std::endl;
	return paths;
}
	
bool Busqueda::buscarEnIndice(std::string consulta, std::string catalogo) {
	     
	if ( consulta.find('*') == std::string::npos) {
		//busqueda simple
		consulta = Parser::aMinuscSinInvalidos(consulta);
		std::cout<<"Busqueda simple: \""<<consulta<<"\""<<std::endl;
		if (consulta.size() != 0) {
			RegistroIndice reg = Buscador::buscar(consulta, catalogo);
			std::cout<<"Frecuencia: "<<reg.frec<<std::endl;
			if ( reg.frec != 0) {
				//obtener los punteros
				std::string nombre_pun = PATH_RES;
				nombre_pun += catalogo;
				nombre_pun += ".pun";
				std::ifstream arch_punteros(nombre_pun.c_str(), std::ios::in | std::ios::binary);
				if (arch_punteros.good()){
					std::list<uint32_t>* puntDocs = new std::list<uint32_t>;
					Registro::obtenerPunterosEnLista(arch_punteros, reg.pDocs , reg.frec, puntDocs);
					punteros.push_back(puntDocs);
					size++;
					arch_punteros.close();
					return true;
				}
				std::cout<<"error al abrir el arch de punteros: "<<nombre_pun<<std::endl;
				return false;
			}
			std::cout<<"frecuencia igual cero"<<std::endl;
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
	std::string str;
	str = '$';
	size_t pos = 0;
	size_t where = 0;
	RegistroNGrama regN;
	do {
		//separo por asteriscos
		where = consulta.find('*', pos);
		str += Parser::aMinuscSinInvalidos(consulta.substr(pos, where - pos));

		if (where == std::string::npos) {
			str +='$';
		}
		std::cout<<"substr: "<<str<<std::endl;
		if (str.size() >= 2){
			//armo bigramas y llamo a buscar para cada uno
			for (size_t car=0; car<(str.size()-1) ;car++) {
				std::cout<<"bigrama: "<<str.substr(car,2)<<std::endl;
				regN = Buscador::buscarNgrama(str.substr(car,2),catalogo);
				//std::cout<<"regN.frec: "<<regN.frec<<"	regN.pun: "<<regN.pDocs<<std::endl;
				//en pDocs esta el offset al archivo con los offsets al indice general
				// entro al indice
				//obtengo el termino, la frec, y el puntero a los docs y los agrego a la lista
				//sort de terminos del bigrama
			}
		}
		else {
			if (str.size() == 1 && str[0] != '$')
				std::cout<<"caracter solo: "<<str[0]<<std::endl;
			else
				std::cout<<"no hay caracteres validos antes del *"<<std::endl;
		}
		str.clear();
		pos = where+1;
	}while(where != std::string::npos);

	//AND de todos los terminos de los bigramas
	//obtengo la lista de punteros de cada termino
	//chequear falsos positivos
	//"mostrar" termino y doc en el que aparece
	//agregarlos docs al vector punteros

	return false;
}

std::string Busqueda::buscarPath(uint32_t puntero,std::string catalogo ) {

	std::string path;
	uint32_t par[2]; //par offsetLex - numDir
	std::string nombre = IDX_ARCH;
	nombre +=catalogo;

	std::ifstream indiceDocs(nombre.c_str(),  std::ios::in | std::ios::binary);
	if (indiceDocs.good()) {
		//obtengo el puntero al lexico de archivos
	     indiceDocs.seekg(puntero*2*sizeof(uint32_t));
		indiceDocs.read((char*)par, 2*sizeof(uint32_t));
		indiceDocs.close();

		nombre = LEX_ARCH;
		nombre += catalogo;
		std::ifstream lexDocs(nombre.c_str(), std::ios::in);
		if(lexDocs.good()) {
			//obtengo el nombre del documento
			lexDocs.seekg(par[0]);
			std::getline(lexDocs, nombre, '\0');
			lexDocs.close();

			//busco el directorio

			lexDocs.open(IDX_DIRS, std::ios::in | std::ios::binary);
			if(lexDocs.good()){
				//obtengo el offset del directorio
				uint32_t offset;
				lexDocs.seekg(4*par[1]);
				lexDocs.read((char*)&offset, sizeof(uint32_t));
				lexDocs.close();

				lexDocs.open(LEX_DIRS,  std::ios::in);
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

void Busqueda::borrarListas() {
	for (unsigned int i = 0; i<size; i++) {
//		delete punteros[i];
	}
	size = 0;
	punteros.clear();
	punteros_match.clear();
}

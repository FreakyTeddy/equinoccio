#include "Parser.h"

/****************************************************************************/
Parser::Parser(uint32_t cantMaxReg) {
	this->archivos= 0;
	this->cantMaxReg= cantMaxReg;
	this->cantReg= 0;
}

/*--------------------------------------------------------------------------*/
Parser::~Parser() {
     StopWord vacio("");
     StopWord *aux;
     for(aux=arbol.RemoverMayorIgual(vacio);aux!=NULL;aux=arbol.RemoverMayorIgual(vacio)){
	  delete aux;
     }
}

/*--------------------------------------------------------------------------*/
void Parser::cargarStopWord(const std::string& nombreStopWord) {
	
	std::fstream archivo(nombreStopWord.c_str(), std::fstream::in);
		
	archivo.seekg(0, std::ios::end);
  	std::streampos	length= archivo.tellg();
  	archivo.seekg(0, std::ios::beg);
		
	char buffer[512];

	arbol.LiberarTodo(true);
	if(archivo.is_open()) {
		while(archivo.tellg() != length) {
			archivo.getline(buffer, 512);
			std::string palabra(buffer);
			arbol.Insert(new StopWord(palabra));
		}
	}

	archivo.close();
}

/*--------------------------------------------------------------------------*/
bool Parser::esStopWord(const std::string& palabra) {
	return(arbol.Search(StopWord(palabra)));
}

/*--------------------------------------------------------------------------*/
std::string Parser::aMinuscSinInvalidos(const std::string& informacion) {
	
	std::string invalidos("!#$%&'()*+,-.0123456789:;<=>?@[]^_`{|}~/\\\"\n");
	std::string palabraFiltrada;
	for(size_t c=0; c<informacion.size(); c++) {
		
		size_t pos= invalidos.find(informacion[c], 0);
		
		if(pos == std::string::npos)
			palabraFiltrada+= tolower(informacion[c]);
		else
			palabraFiltrada+= " "; 
	}

	return palabraFiltrada;
}


/*--------------------------------------------------------------------------*/
void Parser::guardarPalabras(const std::string& palabras) {
	
	std::string strAparsear(palabras); 
	size_t found, pos= 0;
	
	do {
		found= strAparsear.find(" ", pos);
		std::string palabraAgregar(strAparsear, pos, found-pos);
		if(palabraAgregar.compare("") != 0)
			lista.push_back(palabraAgregar);	
		pos= found+1;
	} while(found != std::string::npos);
	
}

/*--------------------------------------------------------------------------*/
uint32_t Parser::getCantArchivosParseados() {
	
	return archivos;
}

/*--------------------------------------------------------------------------*/
uint32_t Parser::getCantReg() {
	
	return cantReg;
}		

/****************************************************************************/

bool Parser::verificarExtension(const std::string& nombre, const char **validas){
     size_t t =nombre.find_last_of('.');

     if(t >= std::string::npos)
	  return false;

     std::string extMinus(nombre,t);
     Util::aMinusculas(extMinus);

     bool encontrado=false;

     for(int i=0;(validas[i]!=NULL) &&!encontrado;i++){

    	 if(extMinus.compare(validas[i])==0)
	       encontrado=true;
     }

     return encontrado;
}

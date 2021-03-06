#include "ParserGenerico.h"
#include "../../FileManager/FileManager.h"

ParserGenerico::ParserGenerico(uint32_t cantMaxReg):Parser::Parser(cantMaxReg){
}

bool ParserGenerico::parsear(std::string nombre, uint32_t documento){

     if(!verificarExtension(nombre,extensionesValidas))
	  return false;

     std::ifstream entrada(nombre.c_str());

     char c=0;
     bool cr=true;
     if(!entrada.good())
	  return false;
     entrada.get(c);

     std::string termino;

     for(;!entrada.eof() && entrada.good();entrada.get(c)){
	  if(isalnum(c) || c<0){
	       termino += c;
	       cr=false;
	  }
	  else if(!cr){
	       cr=true;
	       Util::aMinusculas(termino);
	       if(termino[0] < '0' || termino[0] > '9')
		    guardarTermino(termino, documento);
	       termino.clear();
	  }
     }

     return true;
}

void ParserGenerico::flush(){
     salida.close();
}

void ParserGenerico::guardarTermino(const std::string& termino, uint32_t documento){
     if(!salida.is_open()){
	  // Siempre parseo en el ultimo segmento
	  std::string nombre( FileManager::obtenerPathBase(FileManager::getCantidadSegmentos()));
	  nombre += nombreDump;
	  nombre += Util::intToString(archivos);
	  salida.open(nombre.c_str(), std::ios::out);
     }

     if(!esStopWord(termino)){
	  Registro r(termino, documento);
	  r.escribir(salida,0);
	  cantReg++;
	  if(cantReg >= cantMaxReg){
	       cantReg=0;
	       salida.close();
	       archivos++;
	       // Siempre parseo en el ultimo segmento
	       std::string nombre( FileManager::obtenerPathBase(FileManager::getCantidadSegmentos()));
	       nombre += nombreDump;
	       nombre += Util::intToString(archivos);
	       salida.open(nombre.c_str(), std::ios::out);
	  }
     }
}

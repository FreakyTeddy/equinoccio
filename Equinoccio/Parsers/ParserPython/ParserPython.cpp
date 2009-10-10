#include "ParserPython.h"
#include "../../Registros/Registro.h"
#include "../../Util/Util.h"

#define PYTHON_DUMP_BASENAME "dump_src_"

ParserPython::ParserPython(uint32_t cantMaxReg):Parser::Parser(cantMaxReg){
     cargarStopWord(PYTHON_STOP_WORD_FILE);
     std::string nombre(PYTHON_DUMP_BASENAME);
     nombre += Util::intToString(archivos);
     salida.open(nombre.c_str(), std::ios:out);
}

bool parsear(std::string nombre, uint32_t documento){

     std::ifstream entrada(nombre.c_str());

     char c;
     bool cr=true;
     if(!entrada.good())
	  return false;
     entrada.get(c);
     
     std::string termino;

     bool literal = false;
     char cliteral=0;
     bool comentario = false;
     int literal_count=1;
     for(;!entrada.eof() && entrada.good();entrada.get(c)){
	  if(literal){ //Salteo strings literales
	       if(c=='\\'){
		    entrada.get(c);
		    continue;
	       }
	       if(c==cliteral && literal_count==1){
		    literal=false;
	       }
	       else if(c==cliteral && literal_count==3){
		    char d,e;
		    entrada.get(d);
		    if(c==d){
			 entrada.get(e);
			 if(c==e)
			      literal=false;
		    }
	       }
	       continue;
	  }
	  if(c=='"' || c=='\''){ //Comienzo de un string
	       char d,e;
	       cliteral=c;
	       entrada.get(d);
	       entrada.get(e);
	       if(c==d && d==e){ // ''' o """
		    literal=true;
		    literal_count = 3;
		    continue;
	       }
	       else if(c==d){ // ''X  o ""X
		    literal=false;
		    c=e; //para no perder el caracter;
		    literal_count = 1;
	       }
	       else if(c==e && d != '\\'){ // 'X' o "X", X!='\'
		    literal=false;
		    continue;
	       }
	       else{
		    literal=true;
		    literal_count=1;
		    continue;
	       }
	  }
	  if(c=='#'){ //Comienzo de un comentario (lo salteo todo)
	       do{
		    entrada.get(c);
	       }while(c != '\n' &&!entrada.eof() && entrada.good());
	  }
	  if(isalnum(c) || (unsigned)c > 192){
	       guardarTermino(termino, documento);
	       termino += c;
	       cr=false;
	  }
	  else if(!cr){
	       cr=true;
	       termino.clear();
	  }
     }
}

void ParserPython::guardarTermino(const std::string& termino, uint32_t documento){
     if(!esStopWord(termino)){
	  Registro r(termino, documento);
	  r.escribir(salida,0);
	  cantReg++;
	  if(cantReg >= cantMaxReg){
	       cantReg=0;
	       salida.close();
	       archivos++;
	       nombre += Util::intToString(archivos);
	       salida.open(nombre.c_str(), std::ios:out);
	  }
     }
}

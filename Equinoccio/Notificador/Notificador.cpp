#include "Notificador.h"
#include "../Equinoccio.h"

uint32_t obtenerEof(std::fstream &archivo) {
	//Leo el fin del IDX_ARCH catalogo
	archivo.seekg(0, std::fstream::end);
	uint32_t eof= archivo.tellg();
	archivo.seekg(0, std::fstream::beg);
	return eof;
}

uint32_t buscarMenorDirectorio(RegistroArchivo &regImg, RegistroArchivo &regTxt, RegistroArchivo &regSrc, RegistroArchivo &regSnd) {
	uint32_t dirMenor= (uint32_t)-1;
	if(regImg.nro_dir < dirMenor)
		dirMenor= regImg.nro_dir;
	if(regTxt.nro_dir < dirMenor)
		dirMenor= regTxt.nro_dir;
	if(regSrc.nro_dir < dirMenor)
		dirMenor= regSrc.nro_dir;
	if(regSnd.nro_dir < dirMenor)
		dirMenor= regSnd.nro_dir;
	return dirMenor;
}

void Notificador::buscarArchivos(std::fstream &archivo, std::fstream &lexico, RegistroArchivo &registro, std::map<ino_t,RegistroDisco> &registrosDisco, uint32_t dirMenor, bool existeDirectorio, uint32_t eof) {

	uint32_t dir;
	//Mientras este en el numero de directorio
	do {
		avanzarArchivo(archivo, lexico, registro);
		dir= registro.nro_dir;

		if(dir == dirMenor && existeDirectorio) {
			//Busco si el inode esta en disco
			int cant= registrosDisco.count(registro.inode);

			//std::cout << "Reg inode: " << registro.inode << "- cant: " << cant << std::endl;

			//Si existe verificar modificaciones
			if(cant != 0) {
				//TODO: Verificar modificaciones
				if(registrosDisco[registro.inode].time != registro.time_stamp)
					std::cout << "Si, Tiene que modificarse" << registrosDisco[registro.inode].nombre << std::endl;
				registrosDisco[registro.inode].tipo= '-';
			} else {
				//std::cout << "Se borro el archivo" << registro.inode << std::endl;
				//TODO: Hay que borrar el archivo
				registrosDisco[registro.inode].tipo= 'b';
				registrosDisco.erase(registro.inode);
			}
		}

	} while(dir == dirMenor && archivo.tellg() != eof);
}

void Notificador::buscarModificaciones() {

	uint32_t segmentos= FileManager::getCantidadSegmentos();
	std::string directorio, lexico_dir;
	std::fstream archDirectorio, archLexicoDir;

	uint32_t eofImg; RegistroArchivo regImg; bool errorImg; std::string dirImg; std::fstream archImgDir; std::string lexImg; std::fstream archImgLex;
	uint32_t eofTxt; RegistroArchivo regTxt; bool errorTxt; std::string dirTxt; std::fstream archTxtDir; std::string lexTxt; std::fstream archTxtLex;
	uint32_t eofSrc; RegistroArchivo regSrc; bool errorSrc; std::string dirSrc; std::fstream archSrcDir; std::string lexSrc; std::fstream archSrcLex;
	uint32_t eofSnd; RegistroArchivo regSnd; bool errorSnd; std::string dirSnd; std::fstream archSndDir; std::string lexSnd; std::fstream archSndLex;

	//Auxiliares
	uint32_t dirMenor;
	std::map<ino_t,RegistroDisco> registrosDisco;
	std::map<std::string,bool> directoriosModificados;
	std::string nombre_arch;
	std::string catalogo;
	bool existeDirectorio;
	uint32_t posActual;


	std::cout << "Segmentos: " << segmentos << std::endl;

	for(uint32_t seg= 0; seg<segmentos; seg++) {
		//Directorio
		directorio= FileManager::obtenerPathIdxDirs(seg);
		archDirectorio.open(directorio.c_str(), std::fstream::in);
		lexico_dir= FileManager::obtenerPathLexDirs(seg);
		archLexicoDir.open(lexico_dir.c_str(), std::fstream::in);

		//Abro todos los directorios de los catalogos
		/*Imagen*/
		errorImg= true;
		dirImg= FileManager::obtenerPathIdxArchCatalogo("image",seg);
		archImgDir.open(dirImg.c_str(), std::fstream::in);
		lexImg= FileManager::obtenerPathLexArchCatalogo("image",seg);
		archImgLex.open(lexImg.c_str(), std::fstream::in);
		if(archImgDir.good() && archImgLex.good()) errorImg= false;

		/*Texto*/
		errorTxt= true;
		dirTxt= FileManager::obtenerPathIdxArchCatalogo("text",seg);
		archTxtDir.open(dirTxt.c_str(), std::fstream::in);
		lexTxt= FileManager::obtenerPathLexArchCatalogo("text",seg);
		archTxtLex.open(lexTxt.c_str(), std::fstream::in);
		if(archTxtDir.good() && archTxtLex.good()) errorTxt= false;

		/*Fuente*/
		errorSrc= true;
		dirSrc= FileManager::obtenerPathIdxArchCatalogo("source",seg);
		archSrcDir.open(dirSrc.c_str(), std::fstream::in);
		lexSrc= FileManager::obtenerPathLexArchCatalogo("source",seg);
		archSrcLex.open(lexSrc.c_str(), std::fstream::in);
		if(archSrcDir.good() && archSrcLex.good()) errorSrc= false;

		/*Audio*/
		errorSnd= true;
		dirSnd= FileManager::obtenerPathIdxArchCatalogo("sound",seg);
		archSndDir.open(dirSnd.c_str(), std::fstream::in);
		lexSnd= FileManager::obtenerPathLexArchCatalogo("sound",seg);
		archSndLex.open(lexSnd.c_str(), std::fstream::in);
		if(archSndDir.good() && archSndLex.good()) errorSnd= false;

		if(!errorImg || !errorTxt || !errorSrc || !errorSnd) {
			//Leo los finales de archivo
			eofImg= obtenerEof(archImgDir);
			eofTxt= obtenerEof(archTxtDir);
			eofSrc= obtenerEof(archSrcDir);
			eofSnd= obtenerEof(archSndDir);

			//Leo el primer registro de cada catalogo
			if(!errorImg) {
				avanzarArchivo(archImgDir, archImgLex, regImg);
				archImgDir.seekg(0, std::fstream::beg);
			}
			if(!errorTxt) {
				avanzarArchivo(archTxtDir, archTxtLex, regTxt);
				archTxtDir.seekg(0, std::fstream::beg);
			}
			if(!errorSrc) {
				avanzarArchivo(archSrcDir, archSrcLex, regSrc);
				archSrcDir.seekg(0, std::fstream::beg);
			}
			if(!errorSnd) {
				avanzarArchivo(archSndDir, archSndLex, regSnd);
				archSndDir.seekg(0, std::fstream::beg);
			}

			while((eofImg != archImgDir.tellg() && !errorImg) || (eofTxt != archTxtDir.tellg() && !errorTxt) || (eofSrc != archSrcDir.tellg() && !errorSrc) || (eofSnd != archSndDir.tellg() && !errorSnd)) {

				if(eofImg == archImgDir.tellg() || errorImg)
					regImg.nro_dir= (uint32_t)-1;
				if(eofTxt == archTxtDir.tellg() || errorTxt)
					regTxt.nro_dir= (uint32_t)-1;
				if(eofSrc == archSrcDir.tellg() || errorSrc)
					regSrc.nro_dir= (uint32_t)-1;
				if(eofSnd == archSndDir.tellg() || errorSnd)
					regSnd.nro_dir= (uint32_t)-1;

				//Busco el directorio menor y cargo todos los archivos en memoria del mismo
				dirMenor= buscarMenorDirectorio(regImg, regTxt, regSrc, regSnd);
				//std::cout << "dirMenor: " << dirMenor << std::endl;

				std::string nombreDir;
				existeDirectorio= cargarRegistrosDelDirectorio(dirMenor, archDirectorio, archLexicoDir, registrosDisco, directoriosModificados, nombreDir);

				if(!existeDirectorio){ // Lo tengo que dar de baja en este segmento
				     Equinoccio::eliminarDirectorio(nombreDir);

				}

				//Busco por los catalogos comparando con los archivos levantados de disco
				if(eofImg != archImgDir.tellg() && !errorImg)
					buscarArchivos(archImgDir, archImgLex, regImg, registrosDisco, dirMenor, existeDirectorio, eofImg);
				if(eofTxt != archTxtDir.tellg() && !errorTxt)
					buscarArchivos(archTxtDir, archTxtLex, regTxt, registrosDisco, dirMenor, existeDirectorio, eofTxt);
				if(eofSrc != archSrcDir.tellg() && !errorSrc)
					buscarArchivos(archSrcDir, archSrcLex, regSrc, registrosDisco, dirMenor, existeDirectorio, eofSrc);
				if(eofSnd != archSrcDir.tellg() && !errorSnd)
					buscarArchivos(archSndDir, archSndLex, regSnd, registrosDisco, dirMenor, existeDirectorio, eofSnd);

				//Retrocedo catalogos, si no es el fin del archivo
				if(archImgDir.tellg() == eofImg && regImg.nro_dir == dirMenor)
					errorImg= true;
				else if(!errorImg) {
					posActual= archImgDir.tellg();
					archImgDir.seekg(posActual-RegistroArchivo::size(), std::fstream::beg);
				}
				if(archTxtDir.tellg() == eofTxt && regTxt.nro_dir == dirMenor)
					errorTxt= true;
				else if(!errorTxt) {
					posActual= archTxtDir.tellg();
					archTxtDir.seekg(posActual-RegistroArchivo::size(), std::fstream::beg);
				}
				if(archSrcDir.tellg() == eofSrc && regSrc.nro_dir == dirMenor)
					errorSrc= true;
				else if(!errorSrc) {
					posActual= archSrcDir.tellg();
					archSrcDir.seekg(posActual-RegistroArchivo::size(), std::fstream::beg);
				}
				if(archSndDir.tellg() == eofSnd && regSnd.nro_dir == dirMenor)
					errorSnd= true;
				else if(!errorSnd) {
					posActual= archSndDir.tellg();
					archSndDir.seekg(posActual-RegistroArchivo::size(), std::fstream::beg);
				}

			}// fin catalogos
		} //if

		//Cierro los catalogos
		archImgDir.close();
		archImgLex.close();
		archTxtDir.close();
		archTxtLex.close();
		archSndDir.close();
		archSndLex.close();
		archSrcDir.close();
		archSrcLex.close();
		//Cierro directorio
		archDirectorio.close();
		archLexicoDir.close();
	} //for catalogos

	//Los archivos que nos quedaron en el map registrosDisco son archivos nuevos
	// std::cout << "-------- Archivos Nuevos --------" << std::endl;
	if(!registrosDisco.empty()) {
		std::map<ino_t,RegistroDisco>::iterator it;
		for(it= registrosDisco.begin(); it != registrosDisco.end(); it++)
			if(registrosDisco[it->first].tipo == 'a')
				std::cout << "alta: " << it->first << std::endl;
	}
	registrosDisco.clear();
	//std::cout << "-----------------------------------" << std::endl;

	//Los directorios modificados(agregados o eliminados)
	// std::cout << "++++++ Directorios Modificados ++++++" << std::endl;
	if(!directoriosModificados.empty()) {
		//PRUEBA
		std::map<std::string,bool>::iterator it;
		for(it= directoriosModificados.begin(); it != directoriosModificados.end(); it++) {
		     if(it->second){
				std::cout << "- Se agrego: " << it->first << std::endl;
		     }
		     else{
				std::cout << "- Se elimino: " << it->first << std::endl;
		     }
		}
	}
	// std::cout << "++++++++++++++++++++++++++++++++++" << std::endl;
}

bool Notificador::cargarRegistrosDelDirectorio(uint32_t nro_dir, std::fstream &archDirectorio, std::fstream &archLexicoDir, std::map<ino_t,RegistroDisco> &registrosDisco, std::map<std::string,bool> &directoriosNuevos, std::string& nombreDir) {

	bool existeDirectorio= false;
	RegistroDirectorio regDirectorio;
	RegistroDisco regDisco;
	//Posiciono en el directorio segun numero de directorio leido
	archDirectorio.seekg(nro_dir*RegistroDirectorio::size(),std::ios_base::beg);
	//Leo el puntero al lexico del directorio
	archDirectorio.read((char*)&regDirectorio.pLexico, sizeof(regDirectorio.pLexico));

	//Posiciono en el lexico directorio segun el puntero leido y leo el nombre del directorio
	archLexicoDir.seekg(regDirectorio.pLexico,std::fstream::beg);
	char c=0;
	std::string directorio;
	while((c= archLexicoDir.get()) != 0) {
		directorio+= c;
	}
	nombreDir=directorio;
	std::cout << "* Directorio: " << directorio << std::endl;

	//Elimino de la lista de directorios nuevo
	directoriosNuevos.erase(directorio);

	//Recorro el directorio y lo agrego a un map
	DIR* directory;
	struct dirent* entry;
	struct stat sb;

	if(esDirectorio(directorio)) {
		if((directory =opendir(directorio.c_str())) != NULL) {
			existeDirectorio= true;
			while((entry=readdir(directory))!=NULL) {
				std::string nombreCompleto(directorio+'/'+entry->d_name);
				if(esArchivo(nombreCompleto)) {
					regDisco.nombre= nombreCompleto;
		//			std::cout << "De disco: " << nombreCompleto << std::endl;
					  if(lstat(nombreCompleto.c_str(), &sb) != -1 && Parsers::validarExtension(nombreCompleto) && registrosDisco.count(sb.st_ino) == 0) {
							  regDisco.time= sb.st_mtime;
							  registrosDisco[sb.st_ino]= regDisco;
							  registrosDisco[sb.st_ino].tipo= 'a';
	//						  std::cout << "Inode: " << sb.st_ino << std::endl;
					  }
				} else if(esDirectorio(nombreCompleto) && strncmp(entry->d_name,".",1)!=0) {
					//agrego directorios
					std::cout << "Directorio agregado: " << nombreCompleto << std::endl;
					if(verificarDirectorioNuevo(nombreCompleto))
						directoriosNuevos[nombreCompleto]= true;
				}
			}
			//Cierro el directorio
			closedir(directory);
		}
	}
	if(!existeDirectorio){
	     directoriosNuevos[directorio]= false;
		
	}

	return	existeDirectorio;
}

void Notificador::avanzarArchivo(std::fstream &archArchivo, std::fstream &archLexico, RegistroArchivo &regArchivo) {

	//Leo el registro de datos de un archivo
	archArchivo.read((char*)&regArchivo.pLexico, sizeof(regArchivo.pLexico));
	archArchivo.read((char*)&regArchivo.nro_dir, sizeof(regArchivo.nro_dir));
	archArchivo.read((char*)&regArchivo.time_stamp, sizeof(regArchivo.time_stamp));
	archArchivo.read((char*)&regArchivo.inode, sizeof(regArchivo.inode));

	//Posiciono en el lexico archivo y leo el nombre del archivo
	archLexico.seekg(regArchivo.pLexico,std::fstream::beg);
	char c=0;
	std::string nombre_arch,s;
	while((c= archLexico.get()) != 0) {
		nombre_arch+= c;
	}

//	std::cout << "Nombre Archivo: " << nombre_arch << std::endl;
}

bool Notificador::verificarDirectorioNuevo(const std::string &directorio) {

	//Recorro el directorio y verifico que contenga archivos que nos interesen
	DIR* directory;
	struct dirent* entry;
	bool existeArchivoAParsear= false;

	if((directory =opendir(directorio.c_str())) != NULL) {
		while((entry=readdir(directory))!=NULL && !existeArchivoAParsear) {
			std::string nombreCompleto(directorio+'/'+entry->d_name);
				if(esArchivo(nombreCompleto) && Parsers::validarExtension(nombreCompleto))
					existeArchivoAParsear= true;
		}
		//Cierro el directorio
		closedir(directory);
	}

	return	existeArchivoAParsear;
}

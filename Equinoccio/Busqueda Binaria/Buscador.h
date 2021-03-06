#ifndef BUSCADOR_H_
#define BUSCADOR_H_

#include <iostream>
#include <fstream>
#include <stdint.h>

/* El tipo de archivo que devuelve como resultado de la busqueda.
 **/
typedef struct RegistroIndice{
	uint32_t pLexico;
	uint32_t frec;
	uint32_t pDocs;
	static uint32_t size(){
		return (sizeof(uint32_t)*3);
	}
	
}RegistroIndice;

typedef struct RegistroNGrama{
	char ngrama[2];
	uint32_t frec;
	uint32_t pDocs;
	static uint32_t sizeofNG(){
		return sizeof(char)*2 + sizeof(uint32_t) + sizeof(uint32_t);
	}
}RegistroNGrama;

/* El Buscador es el encargado de realizar una busqueda binaria.
 **/

class Buscador
{
	public:
		/* Busqueda binaria de un termino sobre un catalogo determinado.
		 * 
		 * @param termino El termino a buscar.
		 * @param catalogo El tipo de catalogo.
		 * @param segmento El numero de segmento.
		 **/
		static RegistroIndice buscar(const std::string& termino, const std::string& catalogo, uint32_t segmento);
		
		static RegistroNGrama buscarNgrama(const std::string ngrama,const std::string& catalogo, uint32_t segmento);

		/*
		 * Dado un termino devuelve su numero de termino en el indice
		 *
		 * @param nro guarda el numero de resultado.
		 *
		 * @return true si encuentra el termino. Devuelve false si no encuentra y el campo nro no tiene significado
		 * */
		static bool buscarNroTermino(const std::string &termino, const std::string &catalogo, uint32_t &nro, uint32_t segmento);

		/**
		 * Dado un nombre de directorio devuelve el numero.
		 * @param nro guarda el numero de directorio
		 * @param pun guarda el puntero al lexico de directorios.
		 * @return true si encuentra el directorio.
		 */
		static bool buscarNroDirectorio(const std::string &path, uint32_t &nro, uint32_t &pun, uint32_t segm);

		/**
		 * @param nro numero de directorio
		 * @param pun numero de documento del primer archivo perteneciente al directorio
		 * @return false si no encontro archivos para ese directorio
		 */
		static bool buscarArchivodeDir(std::fstream &idx_arch, uint32_t nro, uint32_t &pun);

};

#endif /*BUSCADOR_H_*/

#ifndef BUSQUEDARANKEADA_H_
#define BUSQUEDARANKEADA_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <stdint.h>
#include "../Registros/Registro.h"
#include "../Registros/RegistroMatriz.h"
#include "../FileManager/FileManager.h"
#include "../Sort/Sort.h"
#include "../Parsers.h"


class BusquedaRankeada{
	public:

	struct RegConsulta {
		uint32_t nro;
		double 	peso;
		bool operator<(const RegConsulta& r) const{
			return (peso<r.peso)?1:0;
		}
		bool operator>(const RegConsulta& r) const{
			return (peso>r.peso)?1:0;
		}
		int unir(const RegConsulta &r) const {
			return 1;
		}
		RegConsulta(){
			nro=0;
			peso=0;
		}
		RegConsulta(uint32_t n, double p){
			nro=n;
			peso=p;
		}

	};

	/**
	 * Funcion encargada de armar la matriz de cosenos para el catalogo
	 * que recibe como parametro, esta sera utilizada para las consultas
	 * rankeadas.
	 *
	 * @param catalogo Es el catalogo que se esta indexando.
	 * 		  documentos Es la cantidad de documentos totales.
	 * 		  terminos Es la cantidad de terminos totales.
	 * @return -1 en caso de que se produzca un error durante el
	 * 			  procesamiento de los datos.
	 * 			1 en caso de que el armado de la matriz sea exitoso.
	 */
     int armarMatrizCoseno(std::string& catalogo, uint32_t documentos, uint32_t terminos);
     
     /**
      * Recibe los terminos de la consulta y guarda en la lista
      * los documentos con su peso
      * @param segm numero de segmento
      * @return false en caso de error
      */
     static bool coseno(std::string &consulta, std::string catalogo, std::list<RegConsulta*> &arbol, uint32_t segm);
};

#endif /* BUSQUEDARANKEADA_H_ */

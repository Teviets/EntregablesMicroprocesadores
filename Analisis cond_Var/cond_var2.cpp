/*---------------------------------------
* UNIVERSIDAD DEL VALLE DE GUATEMALA
* CC3056 - Programacion de Microprocesadores
* Autor: Ismael Ripoll
* Mod.: KB
* Ver.: 1.01
* Fecha: 2022/08/29
*
* References:
* ------
* "Sincronización: Mutex, Variables condición y Semáforos."  
* Universidad Politécnica de Valencia
* Dpto. de Informática de Sistemas y Computadores
* http://www.gii.upv.es/rtportal/tutorial/06-sincronizacion/06-sincronizacion.html
* ---------------------------------------
* Modelo Productor-Consumidor con Mutex y 
* variables de condición.
*----------------------------------------
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Variables globales
int in, out, cont, buffer[10]; 
pthread_cond_t lleno, vacio; // Variable de condicion
pthread_mutex_t semaf; // Variable mutex 

// Subrutina
/**
 * Cada hilo colocara una condicion de espera mientras que el contador sea 10 luego al buffer le agrega el dato
 * y en in se le asigna el un valor en el cual se toma el previo in se le suma y se calcula el modulo entre 10
 * posteriormente se finaliza la condicion y se desbloquea el mutex por lo que otro hilo puede realizar este proceso
 */
int Escribe(int DATO){
	
  pthread_mutex_lock (& semaf); // Se bloquea el mutex
  
  while (cont == 10) 
    pthread_cond_wait(& vacio, &semaf); // Se evalua la condicion esperando una señal de modificacion
  cont++; 
  
  buffer[in]= DATO;
  in = (in+1) % 10;    // 0 0  1 1  para todos valores menor igual 10... indice 1 valor num 11, ind 2 vn 12...
  pthread_cond_broadcast(& lleno); // Se evalua la condicion
  pthread_mutex_unlock(& semaf); // Se libera el mutex
  
  return 0;
} 
/**
 * Esta funcion permite que 
 */
int Lee(){ 

  int dato;   
  pthread_mutex_lock(& semaf);// Se bloquea el mutex
  
  while (cont == 0) 
    pthread_cond_wait(& lleno, &semaf); // Se evalua la condicion
  cont--; 
  
  dato = buffer[out]; 
  out = (out+1) % 10;
  pthread_cond_broadcast(& vacio);  // Se evalua la condicion
  pthread_mutex_unlock(& semaf); // Se libera el mutex
  return dato; 
} 
/**
 * Esta subrutina imprime "Hijo" y numeros del 1 al 11
 */
 void *productor(void * arg){ 
 
  int i; 
  printf("Hijo\n");
  for (i= 0; i< 12; i++) 
    Escribe(i); 
	usleep(1000);
  pthread_exit(0); 
  
  return NULL;
} 

/**
 * Main
 * 
 */
int main(){ 
  int i; 
  pthread_t hijo;
  in = out = cont = 0;

  pthread_mutex_init(&semaf, NULL); // Inicializa el mutex
  pthread_cond_init(&lleno, NULL);  // Inicializa el primer cond var
  pthread_cond_init(&vacio, NULL); // Inicializa el segundo cond var
  pthread_create(&hijo,NULL,productor,NULL); // Crea el hilo con la funcion productor
  
  usleep(1000); // Duerme el programa 1 segundo
  printf("Padre\n"); // Imprime padre
  // Imprime los resultados de la funcion lee
  for (i= 0; i< 12; i++) 
    printf("%d\n ", Lee());
  exit(0); 
}
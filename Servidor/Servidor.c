#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define MAX_CLI 10
#define PATH_PREG "preguntas.txt"

 typedef struct{
int num_cli;
int puntaje;
char estado[10];
int usado;
float tiempo;
int conectado;
}t_cliente;

typedef struct{
char cad[300];
}t_pregunta;

/***********SETEO DE CONFIG*****************/
int PORT/*=7200*/;
//char path_preg[]="\0";
int time_accept=0;
//int MAX_CLI;
//#define TIME_ACCEPT 20
/**VARIABLES GLOBALES**/
volatile sig_atomic_t exitRequested = 0;
pthread_t hilo[MAX_CLI];
 int cant_preg=0;
/*RECIEN AGREGADOS*/
int cant_cli=0;
int i/*,time_accept=TIME_ACCEPT*/; //Expresado en segundos.
t_cliente clientes[MAX_CLI];
int j=0;

/***************************/
char cad[]="\n***BIENVENIDO AL JUEGO PREGUNTANOS***\n";
char msj[]="La respuesta fue correcta.\n";
char msj2[]="La respuesta fue incorrecta\n";
int listener;//recien agregado
struct sockaddr_storage client;//recien agregado
/**********************/
int open_socket();         
int bind_to_port(int, int);
int mi_atoi(char *cad);
//int Inicializar_Preguntas(char preg[][300], int cant, char* path);
int Inicializar_Preguntas(t_pregunta *preg, int cant, char* path);
int contar_preguntas(char *path);
int respuesta_correcta(char* cad, const char* opcion);
void *thread_client(void *socket);
/**ULT prototipo agregado*/
void *aceptados(void *data);
void timer(int trigger);
void determinar_ganador();
void print_resultados(t_cliente *clientes,int cant_cli);
void mostrar_preguntas(t_pregunta* preg,int cant_preg);
int validar_preguntas(t_pregunta *preg,int cant_preg);
void accept_primer_cli();
void salir(pthread_t* hilo);
void *salida(void *data);
/*****Tratamiento de señales*****/
void signal_handler(int signum);
void server_crashed(int signum);
int leer_config(char *path);

int main(){
  leer_config("config.txt");
   //while(1);
/*****Inicializar preguntas*****/
 cant_preg = contar_preguntas(PATH_PREG);
 //char preguntas[cant_preg][300];
  t_pregunta preguntas[cant_preg];

if(Inicializar_Preguntas(preguntas,cant_preg,PATH_PREG) == -1){
   printf("Error al inicializar preguntas.\n");
   return;
  }
  //mostrar_preguntas(preguntas,cant_preg);
   if(validar_preguntas(preguntas,cant_preg))
    printf("Preguntas validadas.\n");
      else{
         printf("Formato incorrecto de preguntas.\n");
           return;
           }
/*** Variables para el socket***/
//const int PORT=7200;
//char vec[5];
 /*int*/ listener = open_socket();
 
 if(listener == -1){
    printf("Error en listener\n");
    return;
    }

int c = bind_to_port(listener,PORT);
 if(c == -1){
   printf("Error en el bind().\n");
   return;
   }
 if(listen (listener,1) == -1){
  printf("No se puede conectar nadie mas, el máximo numero de clientes en espera son 1\n");
  return;
    }
 printf("Mi ip es: 127.0.0.1\n");
 printf("Enlazado al puerto: %d \n",PORT);
while(1){
cant_cli=0;
signal(SIGINT,server_crashed);
accept_primer_cli();//Esperando al primer cliente
pthread_t hilo_accept;
pthread_create(&hilo_accept,NULL,aceptados,NULL);//Iniciando periodo de aceptacion de clientes.
timer(time_accept);//inicia el timer.
pthread_cancel(hilo_accept);//mato el hilo de accept.

printf("El juego comenzará en breve...\n");
sleep(3);
   
 if(cant_cli>MAX_CLI){
   printf("Error, se supero el maximo numero de clientes permitidos.\n");
   return;}
   //printf("cant_cli: %d\n",cant_cli);

 //pthread_t hilo[cant_cli];
  
 for(i=0;i<cant_cli;i++){

      if(pthread_create(&hilo[i],NULL,thread_client,preguntas)<0){ 
      printf("No se pudo crear el thread\n");
       return;}
      }//for
   /*pthread_t hilo_exit;
   pthread_create(&hilo_exit,NULL,salida,hilo);*/
  //salir(hilo);
/***Para detener al servidor en cualquier momento(Control+c)***/
   signal(SIGINT,signal_handler);


  for(i=0;i<cant_cli;i++)
  pthread_join(hilo[i],NULL);
 
 //pthread_join(hilo_exit,NULL);
 
  printf("\nDeterminando al ganador...\n");
  sleep(2);
  determinar_ganador();

  print_resultados(clientes,cant_cli);

  for(i=0;i<cant_cli;i++)
    send(clientes[i].num_cli,&cant_cli,4,MSG_NOSIGNAL);

 for(i=0;i<cant_cli;i++)
  send(clientes[i].num_cli,clientes,sizeof(t_cliente)*cant_cli,MSG_NOSIGNAL);

  for(i=0;i<cant_cli;i++)
  close(clientes[i].num_cli);
  
  if(cant_cli==0)
   printf("\nNo se conecto ningún cliente.\n");
    }//LLAVE WHILE 1
printf("Cerrando el servidor.\n");

/**************************************************************************/
 /*while(1){
  struct sockaddr_storage client;
  unsigned int addres_size = sizeof(client);
  
  printf("Esperando al cliente\n");
  int connect = accept(listener, (struct sockaddr*) &client, &addres_size);
  printf("connect: %d\n",connect);
  if(c == -1){
         printf("No se puede conectar socket secundario\n");
         }
    printf("Atendiendo al cliente\n");
     pthread_t hilo;
     if(pthread_create(&hilo,NULL,thread_client,(void*) &connect)<0){ 
      printf("No se pudo crear el thread\n");
       return;
     }
      /*pthread_join(hilo,NULL);*/
    //Enviando mensaje de bienvenida
    /*send(connect,cad,strlen(cad),0);

    bzero(vec,5);
    recv(connect,vec,5,0);
    printf("RES= %s\n",vec);

    send(connect,&cant_preg,sizeof(cant_preg),0);//envio de cant_preg
    //msg =NULL;
    int i;
     puntos=0;
    char opcion[10]="\0";
    char fin[5];
    printf("Puntaje actual: %d\n",puntos);   

    for(i=0;i<cant_preg;i++){
       
         send(connect,preguntas[i],strlen(preguntas[i]),0);
         bzero(opcion,10);
         recv(connect,opcion,10,0);
         
         printf("la opción elegida fue: %s\n",opcion);

          if(respuesta_correcta(preguntas[i],opcion)){
             send(connect,msj,strlen(msj),0);
              puntos++;
              }
            else{
             send(connect,msj2,strlen(msj2),0);
                 if(puntos!=0)
                 puntos--;
                 }
           printf("Puntaje actual: %d\n",puntos);
         //recv(connect,vec,5,0);// espero el ok para continuar
        }//for
        bzero(fin,5);
        recv(connect,fin,5,0);
        send(connect,&puntos,4,0);
        //sleep(3);
    close(connect);*/
  //}//while ULTIMO COMENTADO

 close(listener);
 return 0;
}//main

int open_socket(){
  int s = socket(PF_INET,SOCK_STREAM,0);
  if( s == -1)
     printf("Error al abrir socket\n");
     return s;
}

int bind_to_port(int socket, int port){

 struct sockaddr_in name;
 name.sin_family = PF_INET;
 name.sin_port = (in_port_t)htons(port);
 name.sin_addr.s_addr = htonl(INADDR_ANY);
 
 int reuse = 1;
 if(setsockopt(socket,SOL_SOCKET,SO_REUSEADDR,(char*)&reuse,sizeof(int)) == -1){
     printf("No es posible reusar el socket\n");
     return -1;
  }
  int c = bind(socket, (struct sockaddr*) &name,sizeof(name));
  if( c == -1){ 
    printf("No se puede enlazar el socket al puerto : la direccion ya esta en uso\n");
    return c;
   }
}

int mi_atoi(char *cad){
int num=0;
while(*cad){
 
  num=num*10 + *cad -'0';
  cad++;
 }
 return num;
}

int Inicializar_Preguntas(t_pregunta *preg, int cant, char* path){
 FILE *pf=NULL;
 char linea[100];
 int i=-1;
  pf = fopen(path,"r");

  if(pf==NULL){
     printf("Error, el archivo no existe\n");
     return -1;
      }

  
       while(fgets(linea,sizeof(linea),pf)){ 
         if(linea[0]=='P'){
             i++;
             preg[i].cad[0]='\0';
          }
         strcat(preg[i].cad,linea);
           
         }
       
      fclose(pf);
 return 1;
}

int contar_preguntas(char *path){
char linea[100];
int contador=0;
FILE* pf=fopen(path,"r");
if(pf==NULL){
     printf("Error, el archivo no existe\n");
     return -1;
      }

  while(fgets(linea,sizeof(linea),pf)){
       if(linea[0]=='P' && linea[1]==':')
          contador++;
     }
 return contador;
}

int respuesta_correcta(char* cad, const char* opcion){
 
 int valor=0,i;

 valor=valor*10+opcion[0]-'0';

  for(i=0;i<valor;i++){
     cad=strchr(cad,'\n');
     cad++;
    }
 if(*cad == 'R' && *(cad+1) == 'C')
   return 1;

 return 0;
}

void* thread_client(void* data)
{  
   t_pregunta* preguntas=(t_pregunta*)data;
   int k,res;
  
   int s;//=clientes[0].num_cli;

    for(k=0;i<cant_cli;k++){
     if(clientes[k].usado==0){
            clientes[k].usado=1;
            s=clientes[k].num_cli;
            
            break;
           }
    }

   int puntos=0;
   char vec[5]="\0";
   
   //mostrar_preguntas(preguntas,cant_preg);
   
   /*for(i=0;i<cant_cli;i++){
   s=clientes[i].num_cli;*/
   send(s,cad,strlen(cad),MSG_NOSIGNAL);
   //}
    sleep(1);
   /*for(i=0;i<cant_cli;i++){
     s=clientes[i].num_cli;*/
     send(s,&s,4,MSG_NOSIGNAL);
   //}
  
   /*for(i=0;i<cant_cli;i++){
    s=clientes[i].num_cli;*/
    bzero(vec,5);
    res=recv(s,vec,5,0);
       if(res==0){
             strcpy(vec,"NO");
             /*pthread_exit(NULL);*/}
    printf("CONNECT= %s",vec);
    //}
    
    /*for(i=0;i<cant_cli;i++){
     s=clientes[i].num_cli;*/
     send(s,&cant_preg,sizeof(cant_preg),MSG_NOSIGNAL);//envio de cant_preg
     //}
    //msg =NULL;
    int i,j;
     puntos=0;
    char opcion[10]="\0";
    char fin[5];
    float tiempo_total;
    printf("Cliente N°%d --> Puntaje actual: %d\n",s,puntos);   

    for(j=0;j<cant_preg;j++){
         
         /*for(i=0;i<cant_cli;i++){
         s=clientes[i].num_cli;*/
         send(s,preguntas[j].cad,strlen(preguntas[j].cad),MSG_NOSIGNAL);
         /*}
         for(i=0;i<cant_cli;i++){
         s=clientes[i].num_cli;*/
         bzero(opcion,10);
         res=recv(s,opcion,10,0);
         if(res==0){
             printf("El cliente N°%d se ha desconectado.\n",s);
             break;}
         
         /*if(strcmp(opcion,"quit")==0){
          printf("El cliente N°%d se ha desconectado.\n",s);
            pthread_exit(NULL);
           }
           else*/
            printf("la opción elegida fue: %s\n",opcion);

          if(respuesta_correcta(preguntas[j].cad,opcion)){
             send(s,msj,strlen(msj),MSG_NOSIGNAL);
              puntos++;
              }
            else{
             send(s,msj2,strlen(msj2),MSG_NOSIGNAL);
                 if(puntos!=0)
                 puntos--;
                 }
           printf("Cliente N°%d --> Puntaje actual: %d\n",s,puntos);
           sleep(2);
           if(exitRequested==1){
              send(s,"end",3,MSG_NOSIGNAL);
              break;
              }else
                   send(s,"sigue\n",6,MSG_NOSIGNAL);
        }//for
        /*for(i=0;i<cant_cli;i++){
         s=clientes[i].num_cli;*/
         bzero(fin,5);
         recv(s,fin,5,0);   
         //}
       /*for(i=0;i<cant_cli;i++){
         s=clientes[i].num_cli;*/
         send(s,&puntos,4,MSG_NOSIGNAL);
        //}
         res=recv(s,&tiempo_total,sizeof(float),0);
          
         
         for(k=0;k<cant_cli;k++){
         if(clientes[k].num_cli==s){
            clientes[k].puntaje=puntos;
            clientes[k].tiempo=tiempo_total;
                   if(res==0)
                       clientes[k].conectado==0;
                        
                }//if
          }//for
            
}
//FUNCIONES RECIEN AGREGADAS
void *aceptados(void *data)
{
   
   while(1){
  
  //struct sockaddr_storage client;
  unsigned int addres_size = sizeof(client);
  
  printf("Esperando al cliente\n");
  int connect = accept(listener, (struct sockaddr*) &client, &addres_size);
  
  if(connect == -1){
         printf("No se puede conectar socket secundario\n");
         } 
   printf("Cliente aceptado.\n");
   printf("Número de cliente asignado: %d\n",connect);
   clientes[cant_cli].num_cli=connect;
   clientes[cant_cli].puntaje=0;
   clientes[cant_cli].usado=0;
   clientes[cant_cli].tiempo=0;
   clientes[cant_cli].conectado==1;
   cant_cli++;

   if(cant_cli>=MAX_CLI){
   printf("Error, se supero el máximo numero de clientes permitidos.\n");
   return;}

   printf("\nCantidad de clientes conectados: %d\n",cant_cli);
   }//while
}//funcion

void timer(int trigger){

int sec = 0;
printf("\nTienes %d segundos para conectarte!\n",trigger);

clock_t before = clock();
 
do{
  clock_t difference = clock() - before;
  sec = difference / CLOCKS_PER_SEC;
  }while ( sec < trigger );

   printf("\nTiempo finalizado.\n");
}

void determinar_ganador(){
 
 char aux[]="\0";
 float mejor_tiempo;
 int band=0;
 int max_puntaje=0;
for(i=0;i<cant_cli;i++){
  
   if(clientes[i].puntaje>=max_puntaje){
     max_puntaje=clientes[i].puntaje;
    }
  }//primer for
for(i=0;i<cant_cli;i++){
  if(clientes[i].tiempo!=0)
   if(band==0 || clientes[i].tiempo<=mejor_tiempo){
     mejor_tiempo=clientes[i].tiempo;
     band=1;
    }
}

for(i=0;i<cant_cli;i++){
    if(clientes[i].puntaje==max_puntaje && clientes[i].tiempo==mejor_tiempo){
      strcpy(clientes[i].estado,"Ganador");
      printf("Ganador cliente N°%d\n",clientes[i].num_cli);
      }
       else
          strcpy(clientes[i].estado,"Perdedor");

  }//segundo for

}//funcion

void print_resultados(t_cliente *clientes,int cant_cli)
{ int i;
   for(i=0;i<cant_cli;i++){
       //if(clientes[i].conectado==1)
   printf("Cliente N°%d Puntaje final:%d Estado:%s Tiempo total de respuesta:%.2f segundos.\n",clientes[i].num_cli,clientes[i].puntaje,clientes[i].estado,clientes[i].tiempo);
   }

}

void mostrar_preguntas(t_pregunta* preg,int cant_preg){
int j;
for(j=0;j<cant_preg;j++){
        printf("Pregunta: %d\n",j+1);
         printf("%s",preg[j].cad);
        }
}

int validar_preguntas(t_pregunta *preg,int cant_preg){
int j,cont;
char *aux;
   for(j=0;j<cant_preg;j++){
    aux=preg[j].cad;
   cont=0;
     while(*aux){
        aux=strchr(aux,'\n');
        if(*aux=='\0' || aux==NULL)
               return 1;
         aux++;
         
        
   if(*aux=='R' && *(aux+1)=='C')
      cont++;
        }//while
        if(cont>=2)
          return 0;
      }//for
    return 1;
}//funcion

void accept_primer_cli(){
unsigned int addres_size = sizeof(client);
  
  printf("Esperando al cliente\n");
  int connect = accept(listener, (struct sockaddr*) &client, &addres_size);
  
  if(connect == -1){
         printf("No se puede conectar socket secundario\n");
         } 
   printf("Cliente aceptado.\n");
   printf("Número de cliente asignado: %d\n",connect);
   clientes[cant_cli].num_cli=connect;
   clientes[cant_cli].puntaje=0;
   clientes[cant_cli].usado=0;
   clientes[cant_cli].tiempo=0;
   clientes[cant_cli].conectado==1;
   cant_cli++;

   if(cant_cli>=MAX_CLI){
   printf("Error, se supero el máximo numero de clientes permitidos.\n");
   return;}

   printf("\nCantidad de clientes conectados: %d\n",cant_cli);
}

void salir(pthread_t* hilo){
 
 pthread_t hilo_exit;
 pthread_create(&hilo_exit,NULL,salida,hilo);

}

void *salida(void *data){

   pthread_t* hilo=(pthread_t*)data;
   int z;

    char texto[]="\0";
    do{
    printf("\nPara finalizar el juego ingrese la palabra exit: ");
    fflush(stdin);
    scanf("%s",texto);
    }while(strcmp(texto,"exit")!=0);

    for(z=0;z<cant_cli;z++)
     pthread_kill(hilo[z],-9);
    
}

void signal_handler(int signum){

    exitRequested = 1;
   printf("\nFinalizando el juego...\n");
}

void server_crashed(int signum){
int k;
char pid[]="";
 for(k=0;k<cant_cli;k++)
  send(clientes[k].num_cli,"El servidor se ha caido.",24,0);

for(k=0;k<cant_cli;k++)
  close(clientes[k].num_cli);

    close(listener);
   sprintf(pid,"kill %d",getpid());
   system(pid);
}

int leer_config(char *path){
char linea[20];
int u,cant;

FILE* pf=fopen(path,"r");
if(pf==NULL){
     printf("Error, el archivo no existe\n");
     return -1;
      }
  for(u=0;u<2;u++){
      
      fgets(linea,sizeof(linea),pf);
 
      switch(u){
      case 0:
        PORT=atoi(linea);
         break;
     
      case 1:
       time_accept=atoi(linea);
         break;
       }//switch
   }//for
   
   printf("%d/%d/\n",PORT,time_accept);
   return 1;
}//funcion

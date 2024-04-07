#include <../include/consola.h>
#include <readline/readline.h>

void leer_consola()
{
    char *leido;

   while (1) {
       leido = readline("> ");
       if (!strcmp(leido, "")) {
           break;
       }
/* Después acá se ve como procesar las instrucciones*/
       free(leido);
   }
   free(leido);

}
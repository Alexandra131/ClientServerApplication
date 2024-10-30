#include "helpers.h"
#include "parson.h"
#include "requests.h"
#include <arpa/inet.h>
#include <netdb.h>   
#include <netinet/in.h>
#include <stdio.h>   
#include <stdlib.h>
#include <string.h>  
#include <sys/socket.h> 
#include <unistd.h> 

#define MAX_COOKIE_SIZE 256
char **continut;
char **cookies;
char *mesaj;
char *raspuns;
char result[256];
const char *prefix = "/api/v1/tema/auth/";
const char *prefix1 = "/api/v1/tema/library/";
const char *host1 = "34.246.184.49";
const char *content_type = "application/json";
char username[BUFLEN], password[BUFLEN];
char *find_substring(char *haystack, char *needle) {
    while (*haystack) {
        char *start = haystack;
        char *pattern = needle;
        
        while (*haystack && *pattern && *haystack == *pattern) {
            haystack++;
            pattern++;
        }
        
        if (!*pattern) {
            return start; 
        }
        
        haystack = start + 1;
    }
    
    return NULL; 
}

void registerFunc(int sockfd) {
    JSON_Value *valoare = json_value_init_object();
    JSON_Object *obiect = json_value_get_object(valoare);
    printf("username=");
    fgets(username, BUFLEN, stdin);
    username[strlen(username) - 1] = 0;

    json_object_set_string(obiect, "username", username);

    printf("password=");

    fgets(password, BUFLEN, stdin);
    password[strlen(password) - 1] = 0;
    json_object_set_string(obiect, "password", password);

    char *serialized_string = json_serialize_to_string_pretty(valoare);
    size_t serialized_length = strlen(serialized_string) + 1;

    
    continut[0] = (char *)malloc(serialized_length * sizeof(char));
    if (continut[0] == NULL) {
        perror("Unable to allocate memory");
        exit(1);
    }

    memcpy(continut[0], serialized_string, serialized_length);
    snprintf(result, sizeof(result), "%s%s", prefix, "register");
    mesaj = compute_post_request(host1, result,
                               content_type, continut, 1, NULL, 0, NULL);

    send_to_server(sockfd, mesaj);
    raspuns = receive_from_server(sockfd);

    char *resp = find_substring(raspuns, "{");
    JSON_Object *raspuns_obiect = json_value_get_object(json_parse_string(resp));

    if (json_object_get_string(raspuns_obiect, "error")) {
      puts("EROARE: User-ul nu a putut fi inregistrat!");
    } else
      printf("SUCCES: User inregistrat cu succes!\n");
     
}


void loginFunc(int sockfd, char **cookies) {

  JSON_Value *valoare = json_value_init_object();
  JSON_Object *obiect = json_value_get_object(valoare);

  printf("username = ");
  fgets(username, BUFLEN, stdin);
  username[strlen(username) - 1] = 0;
  json_object_set_string(obiect, "username", username);
  
  printf("password = ");
  fgets(password, BUFLEN, stdin);
  password[strlen(password) - 1] = 0;
  json_object_set_string(obiect, "password", password);

  if (strchr(username, ' ') != NULL) {
    printf("EROARE: Avem spatii in nume!\n");
    return;
  }

  if (strstr(cookies[0], "connect")) {
    printf("EROARE: Esti deja logat intr-un cont!\n");
    return;
  }

  char *serialized_string = json_serialize_to_string_pretty(valoare);
  size_t serialized_length = strlen(serialized_string) + 1;
  continut[0] = (char *)malloc(serialized_length * sizeof(char));
  if (continut[0] == NULL) {
    perror("Unable to allocate memory");
    exit(1);
  }
  memcpy(continut[0], serialized_string, serialized_length);
  printf("%s\n",continut[0] );

  snprintf(result, sizeof(result), "%s%s", prefix, "login");
  mesaj = compute_post_request(host1, result,
                               content_type, continut, 1, NULL, 0, NULL);
                               

  send_to_server(sockfd, mesaj);
  raspuns = receive_from_server(sockfd);

  char cookie_buffer[MAX_COOKIE_SIZE + 1]; 

 char *inc = find_substring(raspuns, "connect");
 char *final =  find_substring(raspuns, "; Path");
 int lungime = final - inc;


 if (inc != NULL && final != NULL && lungime > 0 && lungime < MAX_COOKIE_SIZE) {
     strncpy(cookie_buffer, inc, lungime);
    cookie_buffer[lungime] = '\0';
    strcpy(cookies[0], cookie_buffer);
 } else {
    printf("EROARE: Cookie invalid!\n");
    return;
 }
 char *resp = find_substring(raspuns, "{");
 JSON_Object *raspuns_obiect = json_value_get_object(json_parse_string(resp));

    
  if (json_object_get_string(raspuns_obiect, "error")) {
    puts("EROARE: Nu te-ai putut loga!");
  } else {
    printf("SUCCES: Te-ai logat cu succes!\n");
  }
      
}

void enterLibraryFunc(int sockfd, char **cookies, char *token) {
  
      if (token[0]) {
        printf("EROARE: Ai deja acces la biblioteca!\n");
        return;
      }

      snprintf(result, sizeof(result), "%s%s", prefix1, "access");
      mesaj = compute_get_request(host1, result,
                              NULL, cookies, 1, NULL);
                              

      send_to_server(sockfd, mesaj);
      raspuns = receive_from_server(sockfd);

   char *resp = find_substring(raspuns, "{");
   JSON_Object *raspuns_obiect = json_value_get_object(json_parse_string(resp));

    if (json_object_get_string(raspuns_obiect, "error")) {

      puts("EROARE: Nu s-a putut garanta accesul la biblioteca!");
  
      return;
    }
    printf("SUCCES: Ai acces la biblioteca!\n");
    strcpy(token,  json_object_get_string(raspuns_obiect, "token"));
   
}

void getBooksFunc(int sockfd, char *token) {

      snprintf(result, sizeof(result), "%s%s", prefix1, "books");
      mesaj = compute_get_request(
          host1, result, NULL, NULL, 0, token);

      send_to_server(sockfd, mesaj);
      raspuns = receive_from_server(sockfd);

      if (find_substring(raspuns, "[")) {
        puts(find_substring(raspuns, "["));
        return;
      }

      char *resp = find_substring(raspuns, "{");
      JSON_Object *raspuns_obiect = json_value_get_object(json_parse_string(resp));

      if (json_object_get_string(raspuns_obiect, "error")) {
         puts("EROARE: Nu s-au putut gasi carti!");
      }
}
 

void addBookFunc(int sockfd, char *token) {

    JSON_Value *valoare = json_value_init_object();
    JSON_Object *obiect = json_value_get_object(valoare);

    char titlu[BUFLEN];
    printf("title = ");
    fgets(titlu, BUFLEN, stdin);
    titlu[strcspn(titlu, "\n")] = '\0'; 

    char autor[BUFLEN];
    printf("author = ");
    fgets(autor, BUFLEN, stdin);
    autor[strcspn(autor, "\n")] = '\0';

    char gen[BUFLEN];
    printf("genre = ");
    fgets(gen, BUFLEN, stdin);
    gen[strcspn(gen, "\n")] = '\0';

    char nr_pg[BUFLEN];
    printf("page_count = ");
    fgets(nr_pg, BUFLEN, stdin);
    nr_pg[strcspn(nr_pg, "\n")] = '\0';

    char editor[BUFLEN];
    printf("publisher = ");
    fgets(editor, BUFLEN, stdin);
    editor[strcspn(editor, "\n")] = '\0'; 

    int cont = 0;
      if (strlen(titlu) > 1) {
        cont ++;
      }
      if (strlen(autor) > 1) {
        cont ++;
      }
      if (strlen(gen) > 1) {
        cont ++;
      }
      if (strlen(editor) > 1 ) {
        cont ++;
      }
       if (strlen(nr_pg) > 0) {
        cont ++;
      }
      if (atoi(nr_pg) > 0) {
        cont ++;
      }

      if (cont == 6) {

        json_object_set_string(obiect, "title", titlu);
        json_object_set_string(obiect, "author", autor);
        json_object_set_string(obiect, "genre", gen);
        json_object_set_number(obiect, "page_count", atoi(nr_pg));
        json_object_set_string(obiect, "publisher", editor);

        char *serialized_string = json_serialize_to_string_pretty(valoare);
        size_t serialized_length = strlen(serialized_string) + 1;
        continut[0] = (char *)malloc(serialized_length * sizeof(char));
        if (continut[0] == NULL) {
            perror("Unable to allocate memory");
            exit(1);
        }
        memcpy(continut[0], serialized_string, serialized_length);
        snprintf(result, sizeof(result), "%s%s", prefix1, "books");
        mesaj = compute_post_request(
            host1, result, content_type,
            continut, 1, NULL, 0, token);

        send_to_server(sockfd, mesaj);
        raspuns = receive_from_server(sockfd);
        
        char *resp = find_substring(raspuns, "{");
        JSON_Object *raspuns_obiect = json_value_get_object(json_parse_string(resp));
        if (json_object_get_string(raspuns_obiect, "error")) {
             printf("EROARE: Cartea nu a fost adaugata!\n");
        } else {
            printf("SUUCES: Cartea a fost adaugata cu succes!\n");
        }
    } else {
        printf("EROARE: Format incorect!\n");
    }
}


void getBookFunc(int sockfd, char *token) {
    char id[BUFLEN];
    char path[BUFLEN];

    printf("id=");
    fgets(id, BUFLEN, stdin);
    id[strcspn(id, "\n")] = '\0'; // Elimină caracterul newline

    sprintf(path, "%s%s%s", prefix1, "books/", id);

      mesaj = compute_get_request(host1, path, NULL, NULL, 0, token);

      send_to_server(sockfd, mesaj);
      raspuns = receive_from_server(sockfd);

    
      if (find_substring(raspuns, "{")) {
        puts(find_substring(raspuns, "{"));
        return;
      }
      char *resp = find_substring(raspuns, "{");
      JSON_Object *raspuns_obiect = json_value_get_object(json_parse_string(resp));
      if (json_object_get_string(raspuns_obiect, "error")) {
        puts("EROARE: Nu s-au putut gasi carti!");
      }
}

void logoutFunc(int sockfd, char **cookies, char *token) {
     snprintf(result, sizeof(result), "%s%s", prefix, "logout");
   mesaj = compute_get_request(host1, result,
                                    NULL, cookies, 1, NULL);

      send_to_server(sockfd, mesaj);
      raspuns = receive_from_server(sockfd);

       char *resp = find_substring(raspuns, "{");
       JSON_Object *raspuns_obiect = json_value_get_object(json_parse_string(resp));

      if (json_object_get_string(raspuns_obiect, "error")) {
        puts("EROARE: Nu esti logat!");
      } else {
        bzero(cookies[0], BUFLEN);
        bzero(token, BUFLEN);
        printf("SUCCES: Te-ai delogat cu succes!\n");
      }
}


void deleteBookFunc(int sockfd, char *token) {
  
    char id[BUFLEN];
    char path[BUFLEN];

    printf("id=");
    fgets(id, BUFLEN, stdin);
    id[strcspn(id, "\n")] = '\0'; // Elimină caracterul newline

    sprintf(path, "%s%s%s", prefix1, "books/", id);

    mesaj =compute_delete_request(host1, path, NULL, NULL, 1, token);

    send_to_server(sockfd, mesaj);
    raspuns = receive_from_server(sockfd);

    char *resp = find_substring(raspuns, "{");
    JSON_Object *raspuns_obiect = json_value_get_object(json_parse_string(resp));
    if (json_object_get_string(raspuns_obiect, "error")) {
      puts("EROARE: Cartea nu a putut fi stearsa");
    } else {
      printf("SUCCES: Cartea a fost stearsa cu succes!\n");
    }

}

int main(int argc, char *argv[]) {

  int sockfd = open_connection(host1, 8080, AF_INET, SOCK_STREAM, 0);
  char comanda[BUFLEN];
  char *token = calloc(BUFLEN, sizeof(char));

  cookies = calloc(1, sizeof(char *));
  cookies[0] = calloc(BUFLEN, sizeof(char));
  
  continut = calloc(1, sizeof(char *));
  continut[0] = calloc(BUFLEN, sizeof(char));
 
  while (1) {

    fgets(comanda, BUFLEN, stdin);
    snprintf(result, sizeof(result), "%s%s", prefix, "register");
    compute_post_request(host1, result, content_type, continut, 1, NULL, 0, NULL);

    if (strcmp(comanda, "register\n") == 0) {
      registerFunc(sockfd);
      continue;      
    }

    if (strcmp(comanda, "login\n") == 0) {
      loginFunc(sockfd, cookies);
      continue;
    }

    if (strcmp(comanda, "enter_library\n") == 0) {
      enterLibraryFunc(sockfd, cookies, token);
      continue;
    }

    if (strcmp(comanda, "get_books\n") == 0) {
      getBooksFunc(sockfd, token);
      continue;
    }

     if (strcmp(comanda, "get_book\n") == 0) {
      getBookFunc(sockfd, token);
      continue;
    }

    if (strcmp(comanda, "add_book\n") == 0) {
      addBookFunc(sockfd, token);
      continue;
    }
  
    if (strcmp(comanda, "delete_book\n") == 0) {
      deleteBookFunc(sockfd, token);
      continue;
    }

    if (strcmp(comanda, "logout\n") == 0) {
      logoutFunc(sockfd, cookies, token);
    
      continue;
    } 

    if (strcmp(comanda, "exit\n") == 0) {
      close_connection(sockfd);
      exit(0);
    }else {
      printf("EROARE: Comanda nu a fost gasita!\n");
    }
  }
}

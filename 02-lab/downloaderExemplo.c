/*
Compile:
   gcc downloaderExemplo.c -o downloaderExemplo -Wall -Wno-unused-result -Og -g -lcurl
Teste:
   ./downloaderExemplo https://www.vivabonito.com.br/wp-content/uploads/2022/01/Gruta-do-Mimoso-5.jpg mimoso.jpg
Referência:
   https://curl.se/libcurl/c/simple.html
*/

#include<stdio.h>
#include<curl/curl.h>

int main(int argc, char **argv)
{   CURL *curl;
    FILE *fp;
    int resultado;

    curl = curl_easy_init();
    if(curl)
    {   
        if(argc != 3)
        {   printf("ERRO: use o downloader assim:\n\n./downloaderExemplo  URL  NOME_ARQUIVO_LOCAL\n");
            return -1;
        }

        // segundo parâmetro é nome do arquivo a ser gravado
        fp = fopen(argv[2], "wb");

        // primeiro parâmetro é URL a ser baixada
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:60.0) Gecko/20100101 Firefox/81.0");
        curl_easy_setopt(curl, CURLOPT_URL, argv[1]);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

        // faça o download
        resultado = curl_easy_perform(curl);

        if(resultado == CURLE_OK)
            printf("Arquivo baixado corretamente\n");
        else
            printf("ERRO: %s\n", curl_easy_strerror(resultado));

        fclose(fp);
        curl_easy_cleanup(curl);
    }
    return 0;
}
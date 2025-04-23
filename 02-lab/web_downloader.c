#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <curl/curl.h>

#define DEFAULT_N_PROCESSES 4

void criar_nome_arquivo(char *url, char *nome_arquivo) {
    // Ignorar "http://" ou "https://"
    char *inicio = url;
    if (strncmp(url, "http://", 7) == 0) {
        inicio = url + 7;
    } else if (strncmp(url, "https://", 8) == 0) {
        inicio = url + 8;
    }
    
    //copiar URL para nome_arquivo
    strcpy(nome_arquivo, inicio);
    
    //substituir barras e pontos por _
    int ultimo_ponto = -1;
    int tamanho = strlen(nome_arquivo);
    
    // onde esta a posicao do ultimo ponto
    for (int i = 0; i < tamanho; i++) {
        if (nome_arquivo[i] == '.') {
            ultimo_ponto = i;
        }
    }
    
    // substituir '/' e '.' (exceto o último) por '_'
    for (int i = 0; i < tamanho; i++) {
        if (nome_arquivo[i] == '/' || (nome_arquivo[i] == '.' && i != ultimo_ponto)) {
            nome_arquivo[i] = '_';
        }
    }
}

// fazer download de uma URL
int fazer_download(char *url) {
    CURL *curl;
    int resultado;
    int fd;
    char nome_arquivo[512] = {0};

    criar_nome_arquivo(url, nome_arquivo);
    
    curl = curl_easy_init();
    if (!curl) {
        printf("ERRO: Falha ao inicializar curl\n");
        return -1;
    }
    
    fd = open(nome_arquivo, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        printf("ERRO: Não foi possível criar o arquivo %s\n", nome_arquivo);
        curl_easy_cleanup(curl);
        return -1;
    }
    
    // configurar curl
    FILE *fp = fdopen(fd, "wb");
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (X11; Linux x86_64; rv:60.0) Gecko/20100101 Firefox/81.0");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
    
    // realizar download
    resultado = curl_easy_perform(curl);
    
    if (resultado == CURLE_OK) {
        printf("Arquivo %s baixado corretamente\n", nome_arquivo);
    } else {
        printf("ERRO ao baixar %s: %s\n", url, curl_easy_strerror(resultado));
    }
    
    // liberar recursos
    fclose(fp);
    curl_easy_cleanup(curl);
    
    return resultado;
}

// função para processar uma única URL em um processo filho
void processar_url_filho(char *url) {
    // Inicializar curl global no processo filho
    curl_global_init(CURL_GLOBAL_ALL);
    
    // Fazer download
    fazer_download(url);
    
    // Liberar recursos e sair
    curl_global_cleanup();
    exit(0);
}

// Função para processar arquivo de URLs em paralelo com N processos
int processar_arquivo_urls_paralelo(char *nome_arquivo, int n_processos) {
    FILE *arquivo;
    char url[512];
    int processos_ativos = 0;
    int status;
    pid_t pid;
    
    // Abrir arquivo de URLs
    arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("ERRO: Não foi possível abrir o arquivo %s\n", nome_arquivo);
        return -1;
    }
    
    // Ler cada linha e criar processos para fazer download
    while (fgets(url, sizeof(url), arquivo)) {
        // Remover '\n' do final da linha
        int len = strlen(url);
        if (len > 0 && url[len-1] == '\n') {
            url[len-1] = '\0';
        }
        
        if (strlen(url) > 0) {
            // Se já temos N processos rodando, espere algum terminar
            if (processos_ativos >= n_processos) {
                wait(&status);
                processos_ativos--;
            }
            
            // Criar novo processo para fazer download
            pid = fork();
            if (pid == 0) {
                // Processo filho
                fclose(arquivo);  // Fechar o arquivo no filho
                processar_url_filho(url);
                // O filho não chega aqui (exit(0) acima)
            } else if (pid > 0) {
                // Processo pai
                processos_ativos++;
            } else {
                // Erro ao criar processo
                printf("ERRO: Falha ao criar processo para URL %s\n", url);
            }
        }
    }
    
    // Fechar arquivo
    fclose(arquivo);
    
    // Aguardar todos os processos filhos terminarem
    while (processos_ativos > 0) {
        wait(&status);
        processos_ativos--;
    }
    
    return 0;
}

int main(int argc, char **argv) {
    int n_processos = DEFAULT_N_PROCESSES;
    
    // Verificar argumentos
    if (argc < 2) {
        printf("ERRO: Uso incorreto\n");
        printf("Para baixar uma URL: %s URL\n", argv[0]);
        printf("Para baixar URLs de um arquivo: %s -f ARQUIVO [-N NUM_PROCESSOS]\n", argv[0]);
        return -1;
    }
    
    // Inicializar curl global
    curl_global_init(CURL_GLOBAL_ALL);
    
    // Verificar parâmetros
    if (strcmp(argv[1], "-f") == 0) {
        if (argc < 3) {
            printf("ERRO: Nome do arquivo não especificado após -f\n");
            return -1;
        }
        
        // Verificar se há parâmetro -N
        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-N") == 0 && i+1 < argc) {
                n_processos = atoi(argv[i+1]);
                if (n_processos <= 0) n_processos = DEFAULT_N_PROCESSES;
                break;
            }
        }
        
        // Processar arquivo com N processos paralelos
        processar_arquivo_urls_paralelo(argv[2], n_processos);
    } else {
        // Baixar uma única URL
        fazer_download(argv[1]);
    }
    
    // Liberar recursos globais do curl
    curl_global_cleanup();
    
    return 0;
}
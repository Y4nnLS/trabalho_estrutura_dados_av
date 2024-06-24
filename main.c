#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node
{
    int id;
    struct node *next;
};

struct actor
{
    int id;
    char *name;
    struct node *id_movies;
};

struct movie
{
    int id;
    char *title;
    struct node *neighbors;
};

/**
 * Função para escapar caracteres especiais em uma string.
 * Retorna uma nova string com os caracteres escapados.
 */
char *escape_quotes(const char *str)
{
    size_t len = strlen(str);
    char *escaped_str = (char *)malloc(2 * len + 1); // Tamanho máximo possível
    if (!escaped_str)
    {
        perror("Erro ao alocar memória");
        exit(EXIT_FAILURE);
    }

    int j = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '"')
        {
            escaped_str[j++] = '\\'; // Adiciona uma barra invertida antes da aspa
        }
        escaped_str[j++] = str[i];
    }
    escaped_str[j] = '\0';
    return escaped_str;
}

/**
 * Adiciona um filme à lista de filmes de um ator.
 *
 * @param a Apontador para a estrutura do ator.
 * @param id_filme ID do filme a ser adicionado.
 */
void adicionar_filme(struct actor *a, int id_filme)
{
    struct node *novo_nodo = (struct node *)malloc(sizeof(struct node));
    if (!novo_nodo)
    {
        perror("Erro ao alocar memória");
        exit(EXIT_FAILURE);
    }
    novo_nodo->id = id_filme;
    novo_nodo->next = a->id_movies;
    a->id_movies = novo_nodo;
}

/**
 * Libera a memória alocada para uma lista encadeada de nós.
 *
 * @param head Apontador para o início da lista encadeada.
 */
void liberar_lista(struct node *head)
{
    struct node *current = head;
    struct node *next;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
}

/**
 * Libera a memória alocada para o array de atores e suas listas encadeadas.
 *
 * @param ArrayArtistas Apontador para o array de atores.
 * @param count_actors Número de atores no array.
 */
void liberar_memoria_atores(struct actor *ArrayArtistas, int count_actors)
{
    for (int i = 0; i < count_actors; i++)
    {
        free(ArrayArtistas[i].name);
        liberar_lista(ArrayArtistas[i].id_movies);
    }
    free(ArrayArtistas);
}

/**
 * Libera a memória alocada para o array de filmes e suas listas encadeadas.
 *
 * @param ArrayFilmes Apontador para o array de filmes.
 * @param count_movies Número de filmes no array.
 */
void liberar_memoria_filmes(struct movie *ArrayFilmes, int count_movies)
{
    for (int i = 0; i < count_movies; i++)
    {
        free(ArrayFilmes[i].title);
        liberar_lista(ArrayFilmes[i].neighbors);
    }
    free(ArrayFilmes);
}

/**
 * Lê os dados dos atores do arquivo e preenche o array de atores.
 *
 * @param filename Nome do arquivo de entrada contendo os dados dos atores.
 * @param ArrayArtistas Apontador para o array de atores.
 * @param size_ArrayArtistas Tamanho inicial do array de atores.
 * @return Número de atores lidos.
 */
int ler_atores(const char *filename, struct actor **ArrayArtistas, int *size_ArrayArtistas)
{
    FILE *file_atores = fopen(filename, "r");
    if (!file_atores)
    {
        perror("Erro ao abrir tsv de atores");
        exit(EXIT_FAILURE);
    }

    char line[1024];
    fgets(line, sizeof(line), file_atores); // Ignorar linha 1
    printf("Gerando ArrayAtores\n");
    int count_actors = 0;
    while (fgets(line, sizeof(line), file_atores))
    {
        char *id_str = strtok(line, "\t");
        char *primaryName = strtok(NULL, "\t");
        char *birthYear = strtok(NULL, "\t");
        char *deathYear = strtok(NULL, "\t");
        char *primaryProfession = strtok(NULL, "\t");
        char *knownForTitles = strtok(NULL, "\t");

        if (id_str && primaryName)
        {
            if (count_actors >= *size_ArrayArtistas)
            {
                *size_ArrayArtistas *= 2;
                *ArrayArtistas = (struct actor *)realloc(*ArrayArtistas, *size_ArrayArtistas * sizeof(struct actor));
                if (!*ArrayArtistas)
                {
                    perror("Erro ao realocar memória");
                    exit(EXIT_FAILURE);
                }
            }

            (*ArrayArtistas)[count_actors].id = atoi(id_str + 2);
            (*ArrayArtistas)[count_actors].name = strdup(primaryName);
            if (!(*ArrayArtistas)[count_actors].name)
            {
                perror("Erro ao duplicar string");
                exit(EXIT_FAILURE);
            }
            (*ArrayArtistas)[count_actors].id_movies = NULL;

            char *movie_title = strtok(knownForTitles, ",");
            while (movie_title)
            {
                int id_filme_formatado = atoi(movie_title + 2);
                adicionar_filme(&(*ArrayArtistas)[count_actors], id_filme_formatado);
                movie_title = strtok(NULL, ",");
            }
            count_actors++;
        }
    }

    fclose(file_atores);
    printf("Array Atores gerado\n");
    return count_actors;
}

/**
 * Lê os dados dos filmes do arquivo e preenche o array de filmes.
 *
 * @param filename Nome do arquivo de entrada contendo os dados dos filmes.
 * @param ArrayFilmes Apontador para o array de filmes.
 * @param size_ArrayFilmes Tamanho inicial do array de filmes.
 * @return Número de filmes lidos.
 */
int ler_filmes(const char *filename, struct movie **ArrayFilmes, int *size_ArrayFilmes)
{
    FILE *file_filmes = fopen(filename, "r");
    if (!file_filmes)
    {
        perror("Erro ao abrir tsv de filmes");
        exit(EXIT_FAILURE);
    }

    char line_filmes[1024];
    fgets(line_filmes, sizeof(line_filmes), file_filmes); // Ignorar linha 1
    printf("Gerando ArrayFilmes\n");
    int count_movies = 0;
    while (fgets(line_filmes, sizeof(line_filmes), file_filmes))
    {
        char *id_str = strtok(line_filmes, "\t");
        char *titleType = strtok(NULL, "\t");
        char *primaryTitle = strtok(NULL, "\t");
        char *originalTitle = strtok(NULL, "\t");
        char *isAdult = strtok(NULL, "\t");
        char *startYear = strtok(NULL, "\t");
        char *endYear = strtok(NULL, "\t");
        char *runtimeMinutes = strtok(NULL, "\t");
        char *genres = strtok(NULL, "\t");

        if (strcmp(titleType, "movie") == 0)
        {
            if (id_str && primaryTitle)
            {
                if (count_movies >= *size_ArrayFilmes)
                {
                    *size_ArrayFilmes *= 2;
                    *ArrayFilmes = (struct movie *)realloc(*ArrayFilmes, *size_ArrayFilmes * sizeof(struct movie));
                    if (!*ArrayFilmes)
                    {
                        perror("Erro ao realocar memória");
                        exit(EXIT_FAILURE);
                    }
                }

                (*ArrayFilmes)[count_movies].id = atoi(id_str + 2);
                char *escaped_title = escape_quotes(primaryTitle);
                (*ArrayFilmes)[count_movies].title = strdup(escaped_title);
                if (!(*ArrayFilmes)[count_movies].title)
                {
                    perror("Erro ao duplicar string");
                    exit(EXIT_FAILURE);
                }
                (*ArrayFilmes)[count_movies].neighbors = NULL;

                count_movies++;
            }
        }
    }

    fclose(file_filmes);
    printf("Array Filmes gerado\n");
    return count_movies;
}

/**
 * Preenche o array de filmes com os neighbors baseados nos atores.
 *
 * @param ArrayArtistas Array de atores.
 * @param count_actors Número de atores.
 * @param ArrayFilmes Array de filmes.
 * @param count_movies Número de filmes.
 */
void preencher_neighbors(struct actor *ArrayArtistas, int count_actors, struct movie *ArrayFilmes, int count_movies)
{
    for (int ator = 0; ator < count_actors; ator++)
    { // O(n)
        struct node *head = ArrayArtistas[ator].id_movies;
        struct node *next_node = head;
        while (next_node)
        { // O(n)
            int id_filme = next_node->id;
            // busca binaria
            int start = 0, end = count_movies;
            while (1)
            { // O(log m)
                if (start > end)
                {
                    next_node = next_node->next;
                    break;
                }
                int middle = ((start + end) / 2);
                if (ArrayFilmes[middle].id == id_filme)
                { // achou o filme
                    if (ArrayFilmes[middle].neighbors == NULL)
                    {
                        ArrayFilmes[middle].neighbors = head;
                    }
                    else
                    {
                        struct node *current_neighbor = ArrayFilmes[middle].neighbors;
                        while (current_neighbor->next != NULL)
                        {
                            current_neighbor = current_neighbor->next;
                        }
                        struct node *current_head = head;
                        while (current_head != NULL)
                        {
                            // Verifica se o id já existe na lista de neighbors
                            struct node *check_duplicate = ArrayFilmes[middle].neighbors;
                            int duplicate_found = 0;
                            while (check_duplicate != NULL)
                            {
                                if (check_duplicate->id == current_head->id)
                                {
                                    duplicate_found = 1;
                                    break;
                                }
                                check_duplicate = check_duplicate->next;
                            }

                            if (!duplicate_found)
                            {
                                struct node *new_node = (struct node *)malloc(sizeof(struct node));
                                if (!new_node)
                                {
                                    perror("Erro ao alocar memória");
                                    exit(EXIT_FAILURE);
                                }
                                new_node->id = current_head->id;
                                new_node->next = NULL;
                                current_neighbor->next = new_node;
                                current_neighbor = new_node;
                            }

                            current_head = current_head->next;
                        }
                    }
                    break;
                }
                if (ArrayFilmes[middle].id > id_filme)
                {
                    end = middle - 1;
                }
                if (ArrayFilmes[middle].id < id_filme)
                {
                    start = middle + 1;
                }
            }
            if (next_node == NULL)
            {
                break;
            }
            next_node = next_node->next;
        }
    }
    printf("Array Filmes alimentado\n");
}

/**
 * Escreve o grafo no formato DOT em um arquivo.
 *
 * @param filename Nome do arquivo de saída para o grafo DOT.
 * @param ArrayFilmes Array de filmes.
 * @param count_movies Número de filmes.
 */
void escrever_grafo(const char *filename, struct movie *ArrayFilmes, int count_movies)
{
    FILE *output_file = fopen(filename, "w");
    if (!output_file)
    {
        perror("Erro ao abrir arquivo dot");
        exit(EXIT_FAILURE);
    }
    printf("Montando grafo...\n");

    fprintf(output_file, "graph { concentrate=true\n");
    for (int i = 0; i < count_movies; i++)
    {
        char *current_movie_title = ArrayFilmes[i].title;
        struct node *current_node = ArrayFilmes[i].neighbors;

        while (current_node)
        {
            int neighbor_id = current_node->id;
            // busca binaria para encontrar o nome do filme com o id do neighbor
            int start = 0, end = count_movies;
            while (start <= end)
            {
                int middle = (start + end) / 2;
                if (ArrayFilmes[middle].id == neighbor_id)
                {
                    char *neighbor_movie_title = ArrayFilmes[middle].title;
                    fprintf(output_file, " \"%s\" -- \"%s\";\n", current_movie_title, neighbor_movie_title);
                    break;
                }
                if (ArrayFilmes[middle].id > neighbor_id)
                {
                    end = middle - 1;
                }
                else
                {
                    start = middle + 1;
                }
            }
            current_node = current_node->next;
        }
    }

    fprintf(output_file, "}\n");
    printf("Grafo montado e salvo em \"%s\"\n", filename);
    fclose(output_file);
}

int main()
{

    // int max_actors = 10000; // Limite máximo de atores a serem processados
    // int max_movies = 10000; // Limite máximo de filmes a serem processados

    int size_ArrayArtistas = 1;
    struct actor *ArrayArtistas = (struct actor *)malloc(size_ArrayArtistas * sizeof(struct actor));
    if (!ArrayArtistas)
    {
        perror("Erro ao alocar memória");
        exit(EXIT_FAILURE);
    }

    int count_actors = ler_atores("name.basics.tsv", &ArrayArtistas, &size_ArrayArtistas);
    // if (count_actors > max_actors)
    //     count_actors = max_actors; // Limita o número de atores ao máximo definido


    int size_ArrayFilmes = 1;
    struct movie *ArrayFilmes = (struct movie *)malloc(size_ArrayFilmes * sizeof(struct movie));
    if (!ArrayFilmes)
    {
        perror("Erro ao alocar memória");
        exit(EXIT_FAILURE);
    }

    int count_movies = ler_filmes("title.basics.tsv", &ArrayFilmes, &size_ArrayFilmes);
    // if (count_movies > max_movies)
    //     count_movies = max_movies; // Limita o número de filmes ao máximo definido

    preencher_neighbors(ArrayArtistas, count_actors, ArrayFilmes, count_movies);

    escrever_grafo("input.dot", ArrayFilmes, count_movies);

    liberar_memoria_atores(ArrayArtistas, count_actors);
    liberar_memoria_filmes(ArrayFilmes, count_movies);

    printf("Fim\n");

    return 0;
}

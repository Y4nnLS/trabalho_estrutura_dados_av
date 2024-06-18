#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
    int id;
    struct node *next;
};

struct actor {
    int id;
    char *name;
    struct node *id_movies;
};

struct movie {
    int id;
    char *title;
    struct node *neighbors;
};

void adicionar_filme(struct actor *a, int id_filme) {
    struct node *novo_nodo = (struct node*) malloc(sizeof(struct node));
    novo_nodo->id = id_filme;
    novo_nodo->next = a->id_movies;
    a->id_movies = novo_nodo;
}

int main() {
    int size_ArrayArtistas = 1;
    struct actor *ArrayArtistas = (struct actor *) malloc(size_ArrayArtistas * sizeof(struct actor));

    FILE *file_atores = fopen("name.basics.tsv", "r");
    if (!file_atores) {
        printf("\nErro ao abrir tsv de atores\n");
        return 1;
    }

    char line[1024];
    fgets(line, sizeof(line), file_atores); // Ignorar linha 1
    printf("Gerando ArrayAtores\n");
    int count_actors = 0;
    while (fgets(line, sizeof(line), file_atores)) {
        char *id_str = strtok(line, "\t");
        char *name = strtok(NULL, "\t");
        char *birthYear = strtok(NULL, "\t");
        char *deathYear = strtok(NULL, "\t");
        char *primaryProfession = strtok(NULL, "\t");
        char *knownForTitles = strtok(NULL, "\t");

        if (id_str && name && knownForTitles) {
            if (count_actors >= size_ArrayArtistas) {
                size_ArrayArtistas *= 2;
                ArrayArtistas = (struct actor *) realloc(ArrayArtistas, size_ArrayArtistas * sizeof(struct actor));
            }

            ArrayArtistas[count_actors].id = atoi(id_str + 2);
            ArrayArtistas[count_actors].name = strdup(name);
            ArrayArtistas[count_actors].id_movies = NULL;

            char *movie_title = strtok(knownForTitles, ",");
            while (movie_title) {
                int id_filme_formatado = atoi(movie_title + 2);
                adicionar_filme(&ArrayArtistas[count_actors], id_filme_formatado);
                movie_title = strtok(NULL, ",");
            }
            count_actors++;
        }
    }

    printf("Array Atores gerado\n");

    int size_ArrayFilmes = 1;
    struct movie *ArrayFilmes = (struct movie *) malloc(size_ArrayFilmes * sizeof(struct movie));

    FILE *file_filmes = fopen("title.basics.tsv", "r");
    if (!file_filmes) {
        printf("\nErro ao abrir tsv de filmes\n");
        return 1;
    }

    char line_filmes[1024];
    fgets(line_filmes, sizeof(line_filmes), file_filmes); // Ignorar linha 1
    printf("Gerando ArrayFilmes\n");
    int count_movies = 0;
    while (fgets(line_filmes, sizeof(line_filmes), file_filmes)) {
        char *id_str = strtok(line_filmes, "\t");
        char *titleType = strtok(NULL, "\t");
        char *primaryTitle = strtok(NULL, "\t");
        char *originalTitle = strtok(NULL, "\t");
        char *isAdult = strtok(NULL, "\t");
        char *startYear = strtok(NULL, "\t");
        char *endYear = strtok(NULL, "\t");
        char *runtimeMinutes = strtok(NULL, "\t");
        char *genres = strtok(NULL, "\t");

        if (titleType[0] != 's') {
            if (id_str && primaryTitle) {
                if (count_movies >= size_ArrayFilmes) {
                    size_ArrayFilmes *= 2;
                    ArrayFilmes = (struct movie *) realloc(ArrayFilmes, size_ArrayFilmes * sizeof(struct movie));
                }

                ArrayFilmes[count_movies].id = atoi(id_str + 2);
                ArrayFilmes[count_movies].title = strdup(primaryTitle);
                ArrayFilmes[count_movies].neighbors = NULL;

                count_movies++;
            }
        }
    }
    printf("Array Filmes gerado\n");

    fclose(file_atores);
    fclose(file_filmes);

    int id_filme = 0;
    for (int ator = 0; ator < count_actors; ator++) { // O(n)
        struct node *head = ArrayArtistas[ator].id_movies;
        struct node *next_node = head;
        while (next_node) { // O(n)
            id_filme = next_node->id;
            // busca binaria
            int start = 0, end = count_movies;
            while (1) { // O(log m)
                if (start > end) {
                    //printf("ID %d filme nao foi encontrado no array de filmes\n");
                    next_node = next_node->next;
                    break;
                }
                int middle = ((start + end) / 2);
                if (ArrayFilmes[middle].id == id_filme) { // achou o filme
                    if (ArrayFilmes[middle].neighbors == NULL) {
                        ArrayFilmes[middle].neighbors = head;
                    } else {
                        struct node *current_neighbor = ArrayFilmes[middle].neighbors;
                        while (current_neighbor->next != NULL) {
                            current_neighbor = current_neighbor->next;
                        }
                        struct node *current_head = head;
                        while (current_head != NULL) {
                            // Verifica se o id já existe na lista de neighbors
                            struct node *check_duplicate = ArrayFilmes[middle].neighbors;
                            int duplicate_found = 0;
                            while (check_duplicate != NULL) {
                                if (check_duplicate->id == current_head->id) {
                                    duplicate_found = 1;
                                    break;
                                }
                                check_duplicate = check_duplicate->next;
                            }

                            if (!duplicate_found) {
                                struct node *new_node = (struct node *)malloc(sizeof(struct node));
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
                if (ArrayFilmes[middle].id > id_filme) {
                    end = middle - 1;
                }
                if (ArrayFilmes[middle].id < id_filme) {
                    start = middle + 1;
                }
            }
            //printf("ATOR ATUAL -> %d\n", ator);
            if (next_node == NULL) {
                break;
            }
            next_node = next_node->next;
        }
    }
    printf("Array Filmes alimentado\n");

    //Montagem grafo final
    FILE *output_file = fopen("input2.dot", "w");
    if (!output_file) {
        printf("\nErro ao abrir arquivo output.dot\n");
        return 1;
    }
    printf("Montando grafo...\n");

    fprintf(output_file, "graph { concentrate=true\n");
    for (int i = 0; i < count_movies; i++) {
        char *current_movie_title = ArrayFilmes[i].title;
        struct node *current_node = ArrayFilmes[i].neighbors;

        while (current_node) {
            int neighbor_id = current_node->id;
            //busca binaria para encontrar o nome do filme com o id do neighbor
            int start = 0, end = count_movies;
            while (start <= end) {
                int middle = (start + end) / 2;
                if (ArrayFilmes[middle].id == neighbor_id) {
                    char *neighbor_movie_title = ArrayFilmes[middle].title;
                    fprintf(output_file, " \"%s\" -- \"%s\";\n", current_movie_title, neighbor_movie_title);
                    break;
                }
                if (ArrayFilmes[middle].id > neighbor_id) {
                    end = middle - 1;
                } else {
                    start = middle + 1;
                }
            }
            current_node = current_node->next;
        }
    }

    
    fprintf(output_file, "}\n");
    printf("!!!!!!!Grafo montado!!!!!!!\n");
    fclose(output_file);


    printf("Fim\n");

    return 0;
}
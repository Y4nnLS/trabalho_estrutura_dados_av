## 🙋‍♂️ Equipe de desenvolvimento
<table align='center'>
  <tr>
    <td align="center">
        <img style="border-radius: 50%;" src="https://avatars.githubusercontent.com/u/101208372?v=4" width="100px;" alt=""/><br /><sub><b><a href="https://github.com/Y4nnLS">Yann Lucas</a></b></sub></a><br />🤓☝</a></td>
  </table>

# Projeto de Grafos de Filmes e Artistas

Este projeto implementa um programa em C que processa dados de artistas e filmes a partir de arquivos TSV fornecidos pelo IMDB. O objetivo é identificar quais filmes possuem artistas em comum e gerar um grafo onde os vértices representam filmes e as arestas representam artistas em comum entre esses filmes. O grafo é representado como uma lista de adjacências e é exportado no formato DOT, permitindo visualização através do Graphviz.

## Estrutura do Projeto

- `main.c`: Contém a implementação principal do programa.
- `name.basics.tsv`: Arquivo TSV contendo informações sobre os artistas.
- `title.basics.tsv`: Arquivo TSV contendo informações sobre os filmes.
- `output.dot`: Arquivo de saída gerado no formato DOT para visualização do grafo.

## Critérios do Trabalho e Implementação

### (1) Array Dinâmico para Armazenar Artistas
A função `ler_atores` implementa a leitura dos dados dos artistas e armazena-os em um array dinâmico, redimensionado conforme necessário:
```c
int ler_atores(const char *filename, struct actor **ArrayArtistas, int *size_ArrayArtistas) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }

    char line[256];
    int count = 0;
    int capacity = 10;
    *ArrayArtistas = (struct actor *)malloc(capacity * sizeof(struct actor));
    
    // Redimensiona o array se necessário
    while (fgets(line, sizeof(line), file)) {
        if (count >= capacity) {
            capacity *= 2;
            *ArrayArtistas = (struct actor *)realloc(*ArrayArtistas, capacity * sizeof(struct actor));
        }
        
        // Processa a linha lida
        // ...
        count++;
    }

    fclose(file);
    *size_ArrayArtistas = count;
    return 0;
}
```

### (2) Lista de Adjacências para Armazenar os Filmes
A função `ler_filmes` implementa a leitura dos dados dos filmes e armazena-os em um array dinâmico, que é usado como base para a lista de adjacências:
```c
int ler_filmes(const char *filename, struct movie **ArrayFilmes, int *size_ArrayFilmes) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }

    char line[256];
    int count = 0;
    int capacity = 10;
    *ArrayFilmes = (struct movie *)malloc(capacity * sizeof(struct movie));
    
    // Redimensiona o array se necessário
    while (fgets(line, sizeof(line), file)) {
        if (count >= capacity) {
            capacity *= 2;
            *ArrayFilmes = (struct movie *)realloc(*ArrayFilmes, capacity * sizeof(struct movie));
        }
        
        // Processa a linha lida
        // ...
        count++;
    }

    fclose(file);
    *size_ArrayFilmes = count;
    return 0;
}
```

### (3) Struct para os Artistas
A estrutura `struct actor` armazena o ID do artista, o nome e uma lista encadeada de IDs dos filmes em que ele participou:
```c
struct actor {
    int id;
    char *name;
    struct node *id_movies;
};
```

### (4) Struct para os Filmes
A estrutura `struct movie` armazena o ID do filme, o título e uma lista encadeada de vizinhos (outros filmes com artistas em comum):
```c
struct movie {
    int id;
    char *title;
    struct node *neighbors;
};
```

### (5) Leitura do Arquivo de Artistas
A função `ler_atores` lê os dados dos artistas do arquivo `name.basics.tsv` e preenche a struct `actor` correspondente, incluindo a lista de filmes destacados:
```c
int ler_atores(const char *filename, struct actor **ArrayArtistas, int *size_ArrayArtistas) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }

    char line[256];
    int count = 0;
    int capacity = 10;
    *ArrayArtistas = (struct actor *)malloc(capacity * sizeof(struct actor));
    
    while (fgets(line, sizeof(line), file)) {
        if (count >= capacity) {
            capacity *= 2;
            *ArrayArtistas = (struct actor *)realloc(*ArrayArtistas, capacity * sizeof(struct actor));
        }
        
        // Processa a linha lida para preencher a struct actor
        // ...
        count++;
    }

    fclose(file);
    *size_ArrayArtistas = count;
    return 0;
}
```

### (6) Leitura do Arquivo de Filmes
A função `ler_filmes` lê os dados dos filmes do arquivo `title.basics.tsv`, ignorando as linhas que não correspondem a filmes (por exemplo, linhas que não possuem "movie" na coluna `titleType`):
```c
int ler_filmes(const char *filename, struct movie **ArrayFilmes, int *size_ArrayFilmes) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }

    char line[256];
    int count = 0;
    int capacity = 10;
    *ArrayFilmes = (struct movie *)malloc(capacity * sizeof(struct movie));
    
    while (fgets(line, sizeof(line), file)) {
        if (count >= capacity) {
            capacity *= 2;
            *ArrayFilmes = (struct movie *)realloc(*ArrayFilmes, capacity * sizeof(struct movie));
        }
        
        // Processa a linha lida para preencher a struct movie
        if (strstr(line, "movie")) {
            // ...
            count++;
        }
    }

    fclose(file);
    *size_ArrayFilmes = count;
    return 0;
}
```

### (7) Formação do Grafo de Filmes
A função `preencher_neighbors` percorre o array de artistas e forma cliques entre os filmes destacados para cada artista, adicionando arestas entre filmes que compartilham artistas:
```c
void preencher_neighbors(struct actor *ArrayArtistas, int count_actors, struct movie *ArrayFilmes, int count_movies) {
    for (int i = 0; i < count_actors; i++) {
        struct node *movie_node = ArrayArtistas[i].id_movies;
        while (movie_node != NULL) {
            struct node *other_movie_node = movie_node->next;
            while (other_movie_node != NULL) {
                adicionar_vizinho(&ArrayFilmes[movie_node->id], other_movie_node->id);
                adicionar_vizinho(&ArrayFilmes[other_movie_node->id], movie_node->id);
                other_movie_node = other_movie_node->next;
            }
            movie_node = movie_node->next;
        }
    }
}
```

### (8) Otimização com Busca Binária
A implementação inclui uma otimização para acelerar a busca de filmes por ID utilizando busca binária, reduzindo o tempo de execução:
```c
int buscar_filme(struct movie *ArrayFilmes, int size_ArrayFilmes, int id) {
    int start = 0;
    int end = size_ArrayFilmes - 1;
    while (start <= end) {
        int middle = (start + end) / 2;
        if (ArrayFilmes[middle].id == id) {
            return middle;
        } else if (ArrayFilmes[middle].id < id) {
            start = middle + 1;
        } else {
            end = middle - 1;
        }
    }
    return -1;
}
```

### (9) Geração do Arquivo DOT
A função `escrever_grafo` gera o arquivo `output.dot` no formato DOT, utilizando os títulos dos filmes como identificadores e imprimindo as arestas que representam artistas em comum:
```c
void escrever_grafo(const char *filename, struct movie *ArrayFilmes, int count_movies) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return;
    }

    fprintf(file, "graph G {\n");
    for (int i = 0; i < count_movies; i++) {
        struct node *neighbor = ArrayFilmes[i].neighbors;
        while (neighbor != NULL) {
            if (ArrayFilmes[i].id < ArrayFilmes[neighbor->id].id) {
                fprintf(file, "    \"%s\" -- \"%s\";\n", ArrayFilmes[i].title, ArrayFilmes[neighbor->id].title);
            }
            neighbor = neighbor->next;
        }
    }
    fprintf(file, "}\n");

    fclose(file);
}
```

### (10) Utilização de Estruturas e Funções Adicionais

#### Estrutura Node
A estrutura `struct node` é usada para criar listas encadeadas dentro das structs `actor` e `movie`:
```c
struct node {
    int id;
    struct node *next;
};
```

#### Funções Adicionais

- **escape_quotes:** Escapa aspas duplas em strings para garantir um formato DOT válido.
    ```c
    char *escape_quotes(const char *str);


    ```
- **adicionar_vizinho:** Adiciona um filme à lista de vizinhos de outro filme.
    ```c
    void adicionar_vizinho(struct movie *filme, int id_vizinho);
    ```

## Visualização
Para visualizar o grafo gerado, use o Graphviz para converter o arquivo DOT em uma imagem:
```sh
dot -Tsvg input.dot -o output.svg
```

## Estruturas de Dados Utilizadas
- **Array Dinâmico:** Utilizado para armazenar os artistas e filmes.
- **Lista Encadeada:** Utilizada para armazenar os IDs dos filmes em que cada artista participou e para armazenar os vizinhos de cada filme no grafo.
- **Busca Binária:** Utilizada para otimizar a busca de filmes por ID durante a construção do grafo.

## Funções Importantes
- `adicionar_filme`: Adiciona um filme à lista de filmes de um ator.
- `adicionar_vizinho`: Adiciona um vizinho à lista de vizinhos de um filme.
- `liberar_lista`: Libera a memória alocada para uma lista encadeada de nós.
- `liberar_memoria_atores`: Libera a memória alocada para o array de atores e suas listas encadeadas.
- `liberar_memoria_filmes`: Libera a memória alocada para o array de filmes e suas listas encadeadas.
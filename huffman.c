#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    unsigned char byte;
    uint32_t freq;
    struct Node *left;
    struct Node *right;
} Node;

typedef struct {
    Node **data;
    int size;
    int capacity;
} MinHeap;

typedef struct {
    unsigned char bits[256];
    int length;
} Code;

static Node *create_node(unsigned char byte, uint32_t freq, Node *left, Node *right) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) {
        fprintf(stderr, "Erro: memoria insuficiente.\n");
        exit(1);
    }

    node->byte = byte;
    node->freq = freq;
    node->left = left;
    node->right = right;
    return node;
}

static void free_tree(Node *root) {
    if (!root) {
        return;
    }

    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

static int is_leaf(const Node *node) {
    return node && !node->left && !node->right;
}

static MinHeap *create_heap(int capacity) {
    MinHeap *heap = (MinHeap *)malloc(sizeof(MinHeap));
    if (!heap) {
        fprintf(stderr, "Erro: memoria insuficiente.\n");
        exit(1);
    }

    heap->data = (Node **)malloc(sizeof(Node *) * capacity);
    if (!heap->data) {
        fprintf(stderr, "Erro: memoria insuficiente.\n");
        free(heap);
        exit(1);
    }

    heap->size = 0;
    heap->capacity = capacity;
    return heap;
}

static void free_heap(MinHeap *heap) {
    if (!heap) {
        return;
    }

    free(heap->data);
    free(heap);
}

static int compare_nodes(const Node *a, const Node *b) {
    if (a->freq != b->freq) {
        return a->freq < b->freq;
    }
    return a->byte < b->byte;
}

static void heap_swap(Node **a, Node **b) {
    Node *tmp = *a;
    *a = *b;
    *b = tmp;
}

static void heap_push(MinHeap *heap, Node *node) {
    int i;

    if (heap->size >= heap->capacity) {
        fprintf(stderr, "Erro: heap cheio.\n");
        exit(1);
    }

    i = heap->size++;
    heap->data[i] = node;

    while (i > 0) {
        int parent = (i - 1) / 2;
        if (compare_nodes(heap->data[parent], heap->data[i])) {
            break;
        }
        heap_swap(&heap->data[parent], &heap->data[i]);
        i = parent;
    }
}

static Node *heap_pop(MinHeap *heap) {
    Node *min;
    int i = 0;

    if (heap->size == 0) {
        return NULL;
    }

    min = heap->data[0];
    heap->data[0] = heap->data[--heap->size];

    while (1) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (left < heap->size && compare_nodes(heap->data[left], heap->data[smallest])) {
            smallest = left;
        }
        if (right < heap->size && compare_nodes(heap->data[right], heap->data[smallest])) {
            smallest = right;
        }
        if (smallest == i) {
            break;
        }

        heap_swap(&heap->data[i], &heap->data[smallest]);
        i = smallest;
    }

    return min;
}

static Node *build_tree(const uint32_t frequencies[256]) {
    MinHeap *heap = create_heap(512);
    Node *root;
    int i;

    for (i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            heap_push(heap, create_node((unsigned char)i, frequencies[i], NULL, NULL));
        }
    }

    if (heap->size == 0) {
        free_heap(heap);
        return NULL;
    }

    while (heap->size > 1) {
        Node *left = heap_pop(heap);
        Node *right = heap_pop(heap);
        Node *parent = create_node(left->byte < right->byte ? left->byte : right->byte,
                                   left->freq + right->freq, left, right);
        heap_push(heap, parent);
    }

    root = heap_pop(heap);
    free_heap(heap);
    return root;
}

static void build_codes(const Node *root, Code codes[256], unsigned char path[256], int length) {
    if (!root) {
        return;
    }

    if (is_leaf(root)) {
        codes[root->byte].length = length == 0 ? 1 : length;
        if (length == 0) {
            codes[root->byte].bits[0] = 0;
        } else {
            memcpy(codes[root->byte].bits, path, (size_t)length);
        }
        return;
    }

    path[length] = 0;
    build_codes(root->left, codes, path, length + 1);
    path[length] = 1;
    build_codes(root->right, codes, path, length + 1);
}

static int write_u32(FILE *file, uint32_t value) {
    unsigned char bytes[4];
    bytes[0] = (unsigned char)(value & 0xFF);
    bytes[1] = (unsigned char)((value >> 8) & 0xFF);
    bytes[2] = (unsigned char)((value >> 16) & 0xFF);
    bytes[3] = (unsigned char)((value >> 24) & 0xFF);
    return fwrite(bytes, 1, 4, file) == 4;
}

static int read_u32(FILE *file, uint32_t *value) {
    unsigned char bytes[4];
    if (fread(bytes, 1, 4, file) != 4) {
        return 0;
    }

    *value = (uint32_t)bytes[0] |
             ((uint32_t)bytes[1] << 8) |
             ((uint32_t)bytes[2] << 16) |
             ((uint32_t)bytes[3] << 24);
    return 1;
}

static long file_size(FILE *file) {
    long current = ftell(file);
    long size;

    if (current < 0 || fseek(file, 0, SEEK_END) != 0) {
        return -1;
    }

    size = ftell(file);
    if (fseek(file, current, SEEK_SET) != 0) {
        return -1;
    }

    return size;
}

static int count_unique_bytes(const uint32_t frequencies[256]) {
    int count = 0;
    int i;

    for (i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            count++;
        }
    }

    return count;
}

static uint8_t calculate_trash_bits(const uint32_t frequencies[256], const Code codes[256]) {
    uint64_t total_bits = 0;
    int i;

    for (i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            total_bits += (uint64_t)frequencies[i] * (uint64_t)codes[i].length;
        }
    }

    return (uint8_t)((8 - (total_bits % 8)) % 8);
}

static int compress_file(const char *input_path, const char *output_path) {
    uint32_t frequencies[256] = {0};
    Code codes[256];
    unsigned char path[256] = {0};
    FILE *input = fopen(input_path, "rb");
    FILE *output;
    Node *root;
    int unique_count;
    uint8_t trash_bits;
    int ch;
    unsigned char current_byte = 0;
    int bit_count = 0;
    int i;

    if (!input) {
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo de entrada '%s'.\n", input_path);
        return 1;
    }

    memset(codes, 0, sizeof(codes));

    while ((ch = fgetc(input)) != EOF) {
        frequencies[(unsigned char)ch]++;
    }

    root = build_tree(frequencies);
    build_codes(root, codes, path, 0);
    unique_count = count_unique_bytes(frequencies);
    trash_bits = calculate_trash_bits(frequencies, codes);

    output = fopen(output_path, "wb");
    if (!output) {
        fprintf(stderr, "Erro: nao foi possivel criar o arquivo de saida '%s'.\n", output_path);
        fclose(input);
        free_tree(root);
        return 1;
    }

    if (!write_u32(output, (uint32_t)unique_count)) {
        fprintf(stderr, "Erro: falha ao escrever o cabecalho.\n");
        fclose(input);
        fclose(output);
        free_tree(root);
        return 1;
    }

    for (i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            fputc(i, output);
            if (!write_u32(output, frequencies[i])) {
                fprintf(stderr, "Erro: falha ao escrever tabela de frequencias.\n");
                fclose(input);
                fclose(output);
                free_tree(root);
                return 1;
            }
        }
    }

    fputc(trash_bits, output);
    rewind(input);

    while ((ch = fgetc(input)) != EOF) {
        Code code = codes[(unsigned char)ch];
        int bit;

        for (bit = 0; bit < code.length; bit++) {
            current_byte = (unsigned char)((current_byte << 1) | code.bits[bit]);
            bit_count++;

            if (bit_count == 8) {
                fputc(current_byte, output);
                current_byte = 0;
                bit_count = 0;
            }
        }
    }

    if (bit_count > 0) {
        current_byte <<= (8 - bit_count);
        fputc(current_byte, output);
    }

    fclose(input);
    fclose(output);
    free_tree(root);
    return 0;
}

static uint64_t total_original_bytes(const uint32_t frequencies[256]) {
    uint64_t total = 0;
    int i;

    for (i = 0; i < 256; i++) {
        total += frequencies[i];
    }

    return total;
}

static int decompress_file(const char *input_path, const char *output_path) {
    uint32_t frequencies[256] = {0};
    uint32_t unique_count;
    uint8_t trash_bits;
    FILE *input = fopen(input_path, "rb");
    FILE *output;
    Node *root;
    Node *current;
    uint64_t decoded = 0;
    uint64_t total_bytes;
    long payload_start;
    long input_size_value;
    int ch;
    uint32_t i;

    if (!input) {
        fprintf(stderr, "Erro: nao foi possivel abrir o arquivo compactado '%s'.\n", input_path);
        return 1;
    }

    if (!read_u32(input, &unique_count) || unique_count > 256) {
        fprintf(stderr, "Erro: arquivo compactado invalido.\n");
        fclose(input);
        return 1;
    }

    for (i = 0; i < unique_count; i++) {
        int byte = fgetc(input);
        uint32_t freq;

        if (byte == EOF || !read_u32(input, &freq)) {
            fprintf(stderr, "Erro: tabela de frequencias incompleta.\n");
            fclose(input);
            return 1;
        }

        frequencies[(unsigned char)byte] = freq;
    }

    ch = fgetc(input);
    if (ch == EOF || ch < 0 || ch > 7) {
        fprintf(stderr, "Erro: campo de bits de lixo invalido.\n");
        fclose(input);
        return 1;
    }
    trash_bits = (uint8_t)ch;

    payload_start = ftell(input);
    input_size_value = file_size(input);
    if (payload_start < 0 || input_size_value < payload_start) {
        fprintf(stderr, "Erro: nao foi possivel calcular o tamanho do payload.\n");
        fclose(input);
        return 1;
    }

    root = build_tree(frequencies);
    current = root;
    total_bytes = total_original_bytes(frequencies);

    output = fopen(output_path, "wb");
    if (!output) {
        fprintf(stderr, "Erro: nao foi possivel criar o arquivo restaurado '%s'.\n", output_path);
        fclose(input);
        free_tree(root);
        return 1;
    }

    if (total_bytes == 0) {
        fclose(input);
        fclose(output);
        free_tree(root);
        return 0;
    }

    if (is_leaf(root)) {
        for (decoded = 0; decoded < total_bytes; decoded++) {
            fputc(root->byte, output);
        }
        fclose(input);
        fclose(output);
        free_tree(root);
        return 0;
    }

    while ((ch = fgetc(input)) != EOF && decoded < total_bytes) {
        long position_after_read = ftell(input);
        int bits_to_read = 8;
        int bit;

        if (position_after_read == input_size_value && trash_bits > 0) {
            bits_to_read = 8 - trash_bits;
        }

        for (bit = 7; bit >= 8 - bits_to_read && decoded < total_bytes; bit--) {
            int value = (ch >> bit) & 1;
            current = value == 0 ? current->left : current->right;

            if (is_leaf(current)) {
                fputc(current->byte, output);
                decoded++;
                current = root;
            }
        }
    }

    if (decoded != total_bytes) {
        fprintf(stderr, "Erro: payload terminou antes da decodificacao completa.\n");
        fclose(input);
        fclose(output);
        free_tree(root);
        return 1;
    }

    fclose(input);
    fclose(output);
    free_tree(root);
    return 0;
}

static void print_usage(const char *program_name) {
    fprintf(stderr, "Uso:\n");
    fprintf(stderr, "  %s -c arquivo_entrada arquivo_saida.huff\n", program_name);
    fprintf(stderr, "  %s -d arquivo_entrada.huff arquivo_saida\n", program_name);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-c") == 0) {
        return compress_file(argv[2], argv[3]);
    }

    if (strcmp(argv[1], "-d") == 0) {
        return decompress_file(argv[2], argv[3]);
    }

    print_usage(argv[0]);
    return 1;
}

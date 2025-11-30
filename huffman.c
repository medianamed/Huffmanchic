#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Структурочка
typedef struct Node {
    unsigned char symbol;       // символ (байт)
    int freq;                   // частота встречаемости
    struct Node* left;          // левый чилдрик
    struct Node* right;         // правый чилдрик
} Node;

Node* create_node(unsigned char symbol, int freq, Node* left, Node* right);
Node* build_tree_from_frequencies(int freq[256]);
void generate_codes(Node* node, char code[], int depth, char* codes[256]);
void free_tree(Node* node);
void encode_file(char* codes[256]);
void decode_file(Node* root);
int files_equal(const char* file1, const char* file2);

Node* create_node(unsigned char symbol, int freq, Node* left, Node* right){
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL){
        fprintf( stderr, "Memory mistake.\n");
        exit(1);
    }
    node -> symbol = symbol;
    node -> freq = freq;
    node -> left = left;
    node -> right = right;

    return node;

}

Node* build_tree_from_frequencies(int freq[256]) {
    Node* leaves[256];
    int leaf_count = 0;

    // Создаём листья
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            leaves[leaf_count] = create_node((unsigned char)i, freq[i], NULL, NULL);
            leaf_count++;
        }
    }

    // Крайние случаи
    if (leaf_count == 0) {
        return NULL;
    }
    if (leaf_count == 1) {
        return leaves[0];
    }

    // Основной цикл объединения
    while (leaf_count > 1) {
        // Находим два минимума
        int min1 = 0;
        int min2 = 1;
        if (leaves[min1]->freq > leaves[min2]->freq) {
            int temp = min1;
            min1 = min2;
            min2 = temp;
        }

        for (int i = 2; i < leaf_count; i++) {
            if (leaves[i]->freq < leaves[min1]->freq) {
                min2 = min1;
                min1 = i;
            } else if (leaves[i]->freq < leaves[min2]->freq) {
                min2 = i;
            }
        }

        // Объединяем в новый узел
        Node* merged = create_node(0, 
                                   leaves[min1]->freq + leaves[min2]->freq,
                                   leaves[min1], 
                                   leaves[min2]);

        // Заменяем min1 на merged, удаляем min2
        leaves[min1] = merged;
        leaves[min2] = leaves[leaf_count - 1];
        leaf_count--;
    }

    return leaves[0];
}

// Рекурсивно обходит дерево и сохраняет коды в массив codes[]
void generate_codes(Node* node, char code[], int depth, char* codes[256]) {
    // Если это лист — сохраняем код
    if (node->left == NULL && node->right == NULL) {
        code[depth] = '\0';  // завершаем строку
        codes[node->symbol] = strdup(code);  // копируем
        return;
    }

    // Иначе — внутренний узел: идём влево и вправо
    code[depth] = '0';
    generate_codes(node->left, code, depth + 1, codes);

    code[depth] = '1';
    generate_codes(node->right, code, depth + 1, codes);
}

int main() {
    // Массив для хранения частот всех 256 возможных значений байта
    int freq[256] = {0}; // автоматически заполняется нулями

    // Пытаемся открыть файл в бинарном режиме
    FILE *fp = fopen("input.txt", "rb");
    if (fp == NULL) {
        printf("ERROR: Failed to open input.txt\n");
        return 1;
    }

    // Читаем файл по одному байту и увеличиваем счётчик в таблице
    int ch;
    while ((ch = fgetc(fp)) != EOF) {
        // fgetc возвращает int, но значения в диапазоне 0–255 (или EOF)
        if (ch >= 0 && ch < 256) {
            freq[ch]++;
        }
    }
    fclose(fp);

    // Выводим таблицу частот только для символов, которые встретились
    printf("=== My cutie frequency Table ===\n");
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            if (i >= 32 && i <= 126) {
                // Печатаемые ASCII-символы: выводим как 'A', '5', '+'
                printf("'%c' (code %3d): %d time\n", (char)i, i, freq[i]);
            } else {
                // Непечатаемые символы (например, перевод строки, табуляция)
                printf("(code %3d): %d time\n", i, freq[i]);
            }
        }
    }

    printf("pupupu, it works!!!!!!!!\n");

    Node* root = build_tree_from_frequencies(freq);
if (root == NULL) {
    printf("File is empty.\n");
    return 1;
}

// Массив для кодов: все указатели = NULL
char* codes[256] = {0};

// Буфер для формирования кода (макс. длина < 256)
char code_buffer[256];

// Генерируем коды
generate_codes(root, code_buffer, 0, codes);

// Выводим
for (int i = 0; i < 256; i++) {
    if (codes[i] != NULL) {
        if (i >= 32 && i <= 126) {
            printf("'%c': %s\n", (char)i, codes[i]);
        } else {
            printf("(code %d): %s\n", i, codes[i]);
        }
    }
}
    return 0;
}

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
void encode_file(char* codes[256]);
void decode_file(Node* root);
int files_equal(const char* file1, const char* file2);
void free_tree(Node* node);

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

    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            leaves[leaf_count] = create_node((unsigned char)i, freq[i], NULL, NULL);
            leaf_count++;
        }
    }

    if (leaf_count == 0) {
        return NULL;
    }
    if (leaf_count == 1) {
        return leaves[0];
    }

    while (leaf_count > 1) {
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

        Node* combined = create_node(0, 
            leaves[min1]->freq+leaves[min2]->freq,
             leaves[min1],
              leaves[min2]);

        leaves[min1] = combined;
        leaves[min2] = leaves[leaf_count - 1];
        leaf_count--;
    }

    return leaves[0];
}

void generate_codes(Node* node, char code[], int depth, char* codes[256]) {
    if (node->left == NULL && node->right == NULL) {
        code[depth] = '\0'; 
        codes[node->symbol] = strdup(code);  
        return;
    }

    code[depth] = '0';
    generate_codes(node->left, code, depth + 1, codes);

    code[depth] = '1';
    generate_codes(node->right, code, depth + 1, codes);
}


void encode_file(char* codes[256]){
    FILE *in = fopen("input.txt", "rb");
    if (in==NULL){
        fprintf(stderr, "Mistake, impossible to open input.txt\n");
        exit(1);
    }

    FILE *chuff = fopen("compressed.huff", "w");
    if (chuff==NULL){
        fprintf(stderr, "hmm, seems like it wasnt made at all");
        exit(1);
    }
int ch;
while ((ch=fgetc(in))!= EOF){
       if (ch >= 0 && ch < 256 && codes[ch] != NULL){
            fputs (codes[ch], chuff);
    }
}
fclose(in);
fclose(chuff);
}


void decode_file(Node* root) {
    FILE* in = fopen("compressed.huff", "r");
    if (!in) {
        fprintf(stderr, "impossiblle to open compressed.huff for decoding.\n");
        exit(1);
    }

    FILE* out = fopen("decompressed.txt", "wb"); 
    if (!out) {
        fprintf(stderr, "Impossiblle to create decompressed.txt.\n");
        fclose(in);
        exit(1);
    }

    Node* current = root;
    int bit;

    while ((bit = fgetc(in)) != EOF) {
        if (bit == '0') {
            current = current->left;
        } else if (bit == '1') {
            current = current->right;
        } else {
            continue;
        }

        if (current->left == NULL && current->right == NULL) {
            fputc(current->symbol, out);
            current = root; 
        }
    }

    fclose(in);
    fclose(out);
}

int files_equal(const char* file1, const char* file2) {
    FILE* f1 = fopen(file1, "rb");
    FILE* f2 = fopen(file2, "rb");

    if (f1 == NULL || f2 == NULL) {
        if (f1) fclose(f1);
        if (f2) fclose(f2);
        return 0;
    }

    int ch1, ch2;
    while (1) {
        ch1 = fgetc(f1);
        ch2 = fgetc(f2);

        if (ch1 != ch2) {
            fclose(f1);
            fclose(f2);
            return 0;
        }

        if (ch1 == EOF) {
            break;
        }
    }

    fclose(f1);
    fclose(f2);
    return 1;
}

void free_tree(Node* node) {
    if (node == NULL) return;
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

     
int main() {

    int freq[256] = {0}; 

    FILE *fp = fopen("input.txt", "rb");
    if (fp == NULL) {
        printf("impossiblle to open input.txt\n");
        return 1;
    }

    int ch;
    while ((ch = fgetc(fp)) != EOF) {
       
        if (ch >= 0 && ch < 256) {
            freq[ch]++;
        }
    }
    fclose(fp);

    printf("=== My cutie frequency Table ===\n");
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            if (i >= 32 && i <= 126) {
                // Печатаемые адские ASCII-символы
                printf("'%c' (code %3d): %d time\n", (char)i, i, freq[i]);
            } else {
                // Непечатаемые символы 
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


char* codes[256] = {0};


char code_buffer[256];


generate_codes(root, code_buffer, 0, codes);


for (int i = 0; i < 256; i++) {
    if (codes[i] != NULL) {
        if (i >= 32 && i <= 126) {
            printf("'%c': %s\n", (char)i, codes[i]);
        } else {
            printf("(code %d): %s\n", i, codes[i]);
        }
    }
}

encode_file(codes);

decode_file(root);  

if (files_equal("input.txt", "decompressed.txt")) {
    printf("Slayyyy! Files are not diffrenet.\n");
} else {
    printf("Pupupu, files are different((((.\n");
}


for (int i = 0; i < 256; i++) {
    free(codes[i]); 
}

free_tree(root);

    return 0;
}

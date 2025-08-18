#include "training.h"
#include "inference.h"
#include <unistd.h>
#include <getopt.h>
#include <time.h>

int main(int argc, char *argv[]) {
        int opt;
        time_t t;

        struct table table;
        struct vector history;

        FILE *vocab = NULL;
        FILE *train = NULL;
        FILE *model = NULL;

        char *prompt = NULL;
        char *word;
        int token;

        srand(time(&t));

        while ((opt = getopt(argc, argv, "ht:m:v:p:")) != -1) {
                switch (opt) {
                case 'h':
                        printf("Markov's Dumb Chatbot");
                        break;
                case 't':
                        if ((train = fopen(optarg, "r")) == NULL) {
                                perror("Invalid filename for training data");
                                exit(1);
                        }
                        break;
                case 'm':
                        if ((model = fopen(optarg, "r")) == NULL) {
                                perror("Invalid filename for inference model");
                                exit(1);
                        }
                        break;
                case 'v':
                        if ((vocab = fopen(optarg, "r")) == NULL) {
                                perror("Invalid filename for vocabulary file");
                                exit(1);
                        }
                        break;
                case 'p':
                        prompt = optarg;
                        break;
                }
        }

        if (vocab == NULL) {
                perror("Vocabulary file not found");
                exit(1);
        } else {
                load_vocabulary(vocab);
                fclose(vocab);
        }

        if (model == NULL) {
                table_init(&table);
        } else {
                fclose(model);
                perror("Not yet implemented!");
                exit(1);
        }

        if (train != NULL) {
                read_data(&table, train);
                fclose(train);
        }

        if (prompt != NULL) {
                vector_init(&history);

                printf("%s ", prompt);

                word = strtok(prompt, " \n,.!;:");

                while (word != NULL) {
                        token = lookup_tok(word);
                        if (token != -1)
                                vector_push(&history, token);

                        word = strtok(NULL, " \n,.!;:");
                }

                table_pfx(&table);
                while (true) {
                        char* token = next_tok(&history, table);
                        if (token == NULL)
                                break;
                        printf("%s ", token);
                }
                printf("\n");

                vector_free(&history);
        }

        free_vocabulary();
        table_free(&table);

        return 0;
}

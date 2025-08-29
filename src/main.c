#include "training.h"
#include "inference.h"
#include <unistd.h>
#include <getopt.h>
#include <time.h>

int main(int argc, char *argv[]) {
        int opt;
        time_t t;

        UT_array *vocabulary = NULL;
        UT_array *history = NULL;
        UT_array *table = NULL;

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

        if (vocab != NULL) {
                load_vocabulary_file(vocab, vocabulary);
                fclose(vocab);
        } else {
                exit(1);
        }

        if (train != NULL) {
                table_depfx(table);
                read_data(table, train, vocabulary);
                table_pfx(table);
                fclose(train);
        }

        remove_vocabulary_duplicates(vocabulary);

        if (model == NULL) {
                table_init(&table, vocabulary);
        } else {
                fclose(model);
                //load_model(table, model);
                perror("load_model(): Not yet implemented!");
                exit(1);
        }

        if (prompt != NULL) {
                printf("%s ", prompt);

                word = strtok(prompt, " \n,.!;:");

                while (word != NULL) {
                        token = lookup_tok(word, vocabulary);
                        if (token != -1)
                                utarray_push_back(history, &token);

                        word = strtok(NULL, " \n,.!;:");
                }

                while (true) {
                        char* token = next_tok(history, table, vocabulary);
                        if (token == NULL)
                                break;
                        printf("%s ", token);
                }
                printf("\n");
        }

        utarray_free(vocabulary);
        utarray_free(history);
        utarray_free(table);

        return 0;
}

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

                        return 0;
                case 't':
                        if ((train = fopen(optarg, "r")) == NULL) {
                                perror("Invalid filename for training data");
                                exit(1);
                        }
                        break;
                case 'm':
                        if ((model = fopen(optarg, "r+")) == NULL) {
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

        utarray_new(vocabulary, &ut_str_icd);
        utarray_new(history, &ut_int_icd);

        if (train != NULL) {
                load_vocabulary_from_file(train, vocabulary);

                if (vocab != NULL) {
                        load_vocabulary_file(vocab, vocabulary);
                }

                if (model != NULL) {
                        load_vocabulary_from_model(model, vocabulary);
                }

                remove_vocabulary_duplicates(vocabulary);

                table_init(&table, vocabulary);

                if (model != NULL) {
                        rebase_model(table, vocabulary, model);
                }

                table_depfx(table);
                read_data(table, train, vocabulary);
                table_pfx(table);
                fclose(train);

                write_model(table, vocabulary, model);
        } else {
                printf("Loading Model...\n");
                load_model(&table, vocabulary, model);
                printf("Model Loaded!\n");

                if (prompt != NULL) {
                        printf("%s ", prompt);

                        word = strtok(prompt, " \n,.!;:");

                        while (word != NULL) {
                                token = lookup_tok(word, vocabulary);
                                if (token != -1) {
                                        utarray_push_back(history, &token);
                                }

                                word = strtok(NULL, " \n,.!;:");
                        }

                        while ((word = next_tok(history, table, vocabulary))) {
                                printf("%s ", word);
                        }
                        printf("\n");
                }

                write_model(table, vocabulary, fopen("result.txt", "w"));
        }

        fclose(model);

        utarray_free(vocabulary);
        utarray_free(history);
        utarray_free(table);

        return 0;
}

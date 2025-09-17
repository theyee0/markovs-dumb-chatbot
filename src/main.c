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

        UT_array *training_files = NULL;
        FILE **train_ptr = NULL;
        FILE *train = NULL;
        FILE *vocab = NULL;
        FILE *model = NULL;

        char *prompt = NULL;
        char *word;
        int token;

        unsigned int n = 100;

        char *tok;

        srand(time(&t));

        /* Parse command line options */
        while ((opt = getopt(argc, argv, "ht:m:v:p:n:")) != -1) {
                switch (opt) {
                case 'h':
                        printf("Markov's Dumb Chatbot\n");
                        printf("Usage: ./markovs_dumb_chatbot [OPTIONS]...\n");
                        printf("-h			Print this help\n");
                        printf("-t file1[,file2,...]	Specify training file names\n");
                        printf("-v vocab 		Specify vocabulary file name\n");
                        printf("-m model		Specify model file name\n");
                        printf("-p prompt		Specify prompt for inference\n");
                        printf("-n number		Specify number of words generated\n");
                        return 0;
                case 't':
                        utarray_new(training_files, &ut_ptr_icd);

                        tok = strtok(optarg, ",");

                        while (tok != NULL) {
                                if ((train = fopen(tok, "r")) == NULL) {
                                        utarray_free(training_files);
                                        perror("Invalid filename for training data");
                                        exit(1);
                                }

                                utarray_push_back(training_files, &train);

                                tok = strtok(NULL, ",");
                        }
                        break;
                case 'm':
                        if ((model = fopen(optarg, "a+")) == NULL) {
                                perror("Invalid filename for inference model");
                                exit(1);
                        }
                        break;
                case 'n':
                        n = strtol(optarg, NULL, 10);
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

        if (model == NULL) {
                fprintf(stderr, "No model file specified!");
                exit(1);
        }

        utarray_new(vocabulary, &ut_str_icd);
        utarray_new(history, &ut_int_icd);

        if (train != NULL) {
                /* Loads all vocabulary from specified files */
                train_ptr = NULL;
                while ((train_ptr = utarray_next(training_files, train_ptr))) {
                        load_vocabulary_file(*train_ptr, vocabulary);
                }

                if (vocab != NULL) {
                        load_vocabulary_file(vocab, vocabulary);
                }

                load_vocabulary_from_model(model, vocabulary);

                remove_vocabulary_duplicates(vocabulary);

                /* Trains model using provided data */
                table_init(&table, vocabulary);
                rebase_model(table, vocabulary, model);

                table_depfx(table);
                train_ptr = NULL;
                while ((train_ptr = utarray_next(training_files, train_ptr))) {
                        read_data(table, *train_ptr, vocabulary);
                        fclose(*train_ptr);
                }
                table_pfx(table);

                write_model(table, vocabulary, model);
                utarray_free(training_files);
        } else if (prompt != NULL) {
                load_model(&table, vocabulary, model);

                printf("%s ", prompt);

                word = strtok(prompt, " \n,.!;:");

                /* Tokenize prompt */
                while (word != NULL) {
                        token = lookup_tok(word, vocabulary);
                        if (token != -1) {
                                utarray_push_back(history, &token);
                        }

                        word = strtok(NULL, " \n,.!;:");
                }

                /* Keep printing completions while they exist and the max word count has not been reached */
                while (utarray_len(history) < n && (word = next_tok(history, table, vocabulary))) {
                        printf("%s ", word);
                }
                printf("\n");
        }

        fclose(model);

        utarray_free(vocabulary);
        utarray_free(history);
        utarray_free(table);

        return 0;
}

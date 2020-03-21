#include "threads.h"

static pthread_rwlock_t lock = PTHREAD_RWLOCK_INITIALIZER; // Lock for prefixes reading
int dict_counter = 0; // Counter for all processes
FILE *file; // File for all processes
pthread_t* THREADS;

/**
 * Work with prefixes from the standard dictionary (__5000_txt.c)
 */
void * default_thread(void *data) {
    char word[100] = ""; // Prefix
    char dom[100] = ""; // A string like prefix.example.com
    char symb;
    unsigned long word_len;

    while (dict_counter < __5000_txt_len) {
        pthread_rwlock_wrlock(&lock);

        // Read next prefix
        while (1) {
            if (__5000_txt[dict_counter] == 0x0a) { // If it's end of prefix
                word_len = strlen(word);
                if (word[word_len - 1] == '\r') word[word_len - 1] = '\0'; // Delete '\r' in the end
                snprintf(dom, sizeof dom, "%s.%s", word, config->host);
                word[0] = '\0';
                dict_counter++;
                break;
            } else {
                symb = (char) __5000_txt[dict_counter];
                sprintf(word, "%s%c", word, symb);
                dict_counter++;
            }
        }

        pthread_rwlock_unlock(&lock);

        dns_request(dom, config->dns, T_A, 1);
    }
    return NULL;
}

/**
 * Work with prefixes from other file
 */
void * file_thread(void * data) {
    char word[100];
    char dom[100];
    unsigned long word_len;

    while (fgets(word, 100, file) != NULL) {
        pthread_rwlock_wrlock(&lock);

        word_len = strlen(word);
        if (word[word_len - 1] == '\n') word[word_len - 1] = '\0'; // Delete '\n' in the end
        word_len = strlen(word);
        if (word[word_len - 1] == '\r') word[word_len - 1] = '\0'; // Delete '\r' in the end
        snprintf(dom, sizeof dom, "%s.%s", word, config->host);

        pthread_rwlock_unlock(&lock);

        dns_request(dom, config->dns, T_A, 1);
    }
    return NULL;
}

/**
 * Count the number of lines in the file
 */
int file_len() {
    int lines_count = 0;

    while(fscanf(file, "%*[^\n]%*c") != EOF)
        lines_count++;
    if (ferror(file))
        return 0;

    lines_count++;
    return lines_count;
}

/**
 * Runs the desired number of default_thread or file_thread
 * (looks for the presence of the `file` parameter in the config)
 */
void run_threads() {

    if (config->file != NULL) { // Open the file and count the number of lines
        file = fopen(config->file, "r");
        if (file == NULL) {
            printf ("\nCan't open %s", config->file);
            return;
        }
        printf("\nPerforming %d tests...", file_len() - 1);
        fseek(file, 0, SEEK_SET);
    } else { // Working with a standard dictionary
        printf("\nPerforming 5000 tests...");
    }

    THREADS = (pthread_t*) malloc(config->threads * sizeof(pthread_t));

    // Starting threads
    for (int i = 0; i < config->threads; i++)
    {
        if (config->file != NULL) {
            pthread_create(&(THREADS[i]), NULL, file_thread, NULL);
        } else {
            pthread_create(&(THREADS[i]), NULL, default_thread, NULL);
        }
    }
    for(int i = 0; i < config->threads; i++) {
        pthread_join(THREADS[i], NULL);
    }
    free(THREADS);
}

void intHandler(int s) {
    for(int i = 0; i < config->threads; i++) {
        pthread_cancel(THREADS[i]);
    }
}

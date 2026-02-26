#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// take the user text
int api_call(char *prompt, char **output) {
    char cmd[2048];

    
    snprintf(cmd, sizeof(cmd),
        "curl -s http://localhost:11434/api/chat -d '{"
        "\"model\": \"gemma3:1B\","
        "\"messages\": [ { \"role\": \"user\", \"content\": \"%s\" } ],"
        "\"stream\": false}'",
        prompt
    );

   FILE *fp = popen(cmd, "r");
    if (!fp) {
        perror("popen failed");
        return 1;
    }

    char buffer[2048];
    char *result = malloc(1);
    result[0] = '\0';
    size_t total = 0;
// read the response
    while (fgets(buffer, sizeof(buffer), fp)) {
        total += strlen(buffer);
        result = realloc(result, total + 1);
        strcat(result, buffer);
    }

    pclose(fp);
    *output = result;
    return 0;
}
//use to extract the content from API
char* extract_content(char *json) {
    char *p = strstr(json, "\"content\":");
    if (!p) return strdup("(no content found)");

    p = strchr(p + 10, '\"');  
    if (!p) return strdup("(parse error)");
    p++;

    char *q = strchr(p, '\"');
    if (!q) return strdup("(parse error)");

    int len = q - p;
    char *out = malloc(len + 1);
    strncpy(out, p, len);
    out[len] = '\0';
    return out;
}

int main() {
    char user_input[512];

    while (1) {
        printf("\nUser: ");
        fflush(stdout);

        if (!fgets(user_input, sizeof(user_input), stdin)) break;
        user_input[strcspn(user_input, "\n")] = '\0';

        if (strcmp(user_input, "exit") == 0) break;

        char *reply_raw = NULL;
        api_call(user_input, &reply_raw);

        char *ai = extract_content(reply_raw);

        printf("AI: %s\n", ai);

        free(ai);
        free(reply_raw);
    }

    return 0;
}
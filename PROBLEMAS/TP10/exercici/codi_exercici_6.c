#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *nom;
    int *notes;
    int num_notes;
} EstudiantUB2026;

int main(void) {

    EstudiantUB2026 *s = malloc(sizeof(EstudiantUB2026));
    if (!s) return 1;

    s->nom = malloc(20 * sizeof(char));
    strcpy(s->nom, "Linus");

    s->num_notes = 3;
    s->notes = malloc(s->num_notes * sizeof(int));
    for (int i = 0; i < s->num_notes; i++) {
        s->notes[i] = i * 5;
    }

    printf("Estudiant: %s\n", s->nom);
    for (int i = 0; i < s->num_notes; i++) {
        printf("Notes %d: %d\n", i, s->notes[i]);
    }

    free(s);

    return 0;
}

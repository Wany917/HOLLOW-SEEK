#include <stdio.h>
#include <stdlib.h>

void get_syslog_errors_warnings() {
    FILE *fp;
    char path[1035];

    // Exécuter la commande journalctl pour récupérer les erreurs et avertissements
    fp = popen("journalctl -p err -p warning", "r");
    if (fp == NULL) {
        printf("Erreur: Impossible d'exécuter la commande journalctl\n");
        return;
    }

    // Lire et afficher la sortie de la commande
    while (fgets(path, sizeof(path), fp) != NULL) {
        printf("%s", path);
    }

    // Fermer le fichier
    pclose(fp);
}

int main() {
    printf("Erreurs et avertissements du journal système:\n");
    get_syslog_errors_warnings();
    return 0;
}

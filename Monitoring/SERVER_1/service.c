#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fonction pour exécuter une commande et lire sa sortie
void execute_command(const char *command) {
    FILE *fp;
    char path[1035];

    // Exécuter la commande
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Erreur: Impossible d'exécuter la commande\n");
        return;
    }

    // Lire et afficher la sortie de la commande
    while (fgets(path, sizeof(path), fp) != NULL) {
        printf("%s", path);
    }

    // Fermer le fichier
    pclose(fp);
}

// Fonction pour récupérer les erreurs spécifiques aux applications
void get_application_errors(const char *service_name) {
    char command[256];

    // Construire la commande journalctl pour le service spécifié
    snprintf(command, sizeof(command), "journalctl -u %s -p err", service_name);

    printf("Erreurs pour le service %s:\n", service_name);
    execute_command(command);
}

int main() {
    // Nom du service ou de l'application à vérifier
    char service_name[50];

    printf("Entrez le nom du service ou de l'application: ");
    scanf("%s", service_name);

    get_application_errors(service_name);
    return 0;
}

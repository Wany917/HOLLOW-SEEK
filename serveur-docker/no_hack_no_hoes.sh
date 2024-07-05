#!/bin/bash

# Répertoires à surveiller
WATCH_DIRS=(~/py ~/sh ~/exe ~/js ~/txt ~/bat)

# Répertoire de destination pour le déplacement des fichiers
MAL_DIR=~/scan/user_1/malware
CONT_DIR=~/scan/user_1/cont_1

# Répertoire source pour le Dockerfile
DOCKERFILE_SRC=~/dockerfile_av_image/clamav/Dockerfile

# Répertoire du script Pulumi
PULUMI_DIR=~/pulumi_script

# Assurez-vous que les répertoires cont_1 et malware existent
mkdir -p "$MAL_DIR"
mkdir -p "$CONT_DIR"

# Démarrer inotifywait pour surveiller les répertoires
inotifywait -m -e create --format '%w%f' "${WATCH_DIRS[@]}" | while read NEW_FILE
do
    # Extraire le nom du fichier et l'extension
    FILE_NAME=$(basename "$NEW_FILE")
    #FILE_BASENAME="${FILE_NAME%.*}"

    # Initialiser le suffixe
    SUFFIX=0
    NEW_DIR="$MAL_DIR/$FILE_BASENAME"

    # Boucle pour trouver un répertoire non existant
    while [ -d "$NEW_DIR" ]; do
        SUFFIX=$((SUFFIX + 1))
        NEW_DIR="$MAL_DIR/${FILE_NAME}_$SUFFIX"
    done

    # Créer le nouveau répertoire avec le nom du fichier
    mkdir -p "$NEW_DIR"

    # Déplacer le fichier dans le nouveau répertoire
    mv "$NEW_FILE" "$NEW_DIR"

    # Copier le fichier dans cont_1
    cp "$NEW_DIR/$FILE_NAME" "$CONT_DIR/"

    # Copier le Dockerfile dans cont_1
    cp "$DOCKERFILE_SRC" "$CONT_DIR/"

    cd "$PULUMI_DIR"
    source "venv/bin/activate"

    pulumi up --yes >> ~/log_bash/log_pulumi.txt

    #deactivate
    echo "Fichier $FILE_NAME traité et déplacé dans $NEW_DIR et copié dans cont_1 avec le Dockerfile." >> ~/log_bash/log.txt
done

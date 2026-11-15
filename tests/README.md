# Tests de sonomkr-core

## Entrée WAV

`sonomkr-core` peut lire un fichier WAV à la place de la carte son ALSA :

```sh
sonomkr-core --config ./sonomkr.conf --filters ./filters.conf \
             --wav mesure.wav [--wav-speed 1] [--wav-loop]
```

| Option | Rôle |
|---|---|
| `--config`, `--filters` | chemins des fichiers de configuration (sinon `./` ou `/etc/sonomkr/` en root) |
| `--wav <fichier>` | remplace la capture ALSA par le fichier |
| `--wav-speed <x>` | `1` = temps réel (défaut), `4` = 4 fois plus vite, `0` = aussi vite que possible |
| `--wav-loop` | relit le fichier en boucle |

- Formats acceptés : PCM 16, 24 et 32 bits, flottant 32 bits (y compris `WAVE_FORMAT_EXTENSIBLE`).
- La fréquence d'échantillonnage du fichier remplace celle de la configuration
  (seules 44 100 et 48 000 Hz ont des filtres dans `conf/filters.conf`).
- Un fichier mono alimente les deux voies actives.
- Le reste ne change pas : publication ZeroMQ des spectres (`LEQ`) et de l'audio,
  pilotage par le contrôleur (`START`, `STOP`, `GET`, `SET`...).
- La lecture ne démarre qu'avec la première voie (autostart ou commande `START`).
- En vitesse maximale, la seule attente a lieu si les filtres ont 15 s de retard
  (pour ne pas écraser le buffer circulaire de 30 s). Seule la fin du fichier qui
  ne remplit pas un bloc de 10 000 échantillons n'est pas traitée.

> La configuration principale est réécrite par `sonomkr-core` au chargement :
> travailler sur une copie.

Générer un signal de test (stdlib Python uniquement) :

```sh
python3 tests/wavgen.py sinus.wav --rate 48000 --format s24 --duration 10 --tone 1000:94
```

Les niveaux sont en dB SPL avec la convention du logiciel :
1,0 pleine échelle = 1 V, sensibilité en V/Pa (0,05 par défaut).

## Suite de tests

| Test | Contenu |
|---|---|
| `wavreader` | test unitaire C++ du lecteur WAV (formats, chunks, fichiers invalides) |
| `e2e_*` | WAV synthétique → `sonomkr-core` → spectres reçus par ZeroMQ → comparaison aux niveaux attendus |

Les scénarios de bout en bout sont dans `e2e_wav.py` (`SCENARIOS`) : sinus 1 kHz
dans les 4 formats, stéréo avec un signal différent par voie, multi-sons de 2 Hz à
10 kHz à 48 kHz et 44,1 kHz, voie 2 seule, intégration courte (0,125 s), fichier
de 60 s, lecture cadencée ×4. Critères :

- bande d'un son pur : niveau moyen à ±0,5 dB du niveau injecté ;
- bandes situées à 3 tiers d'octave ou plus de tout son : au moins 30 dB sous le son
  le plus fort (25 dB pour les bandes inférieures à 2 Hz). C'est un contrôle de
  vraisemblance, pas une vérification de classe selon la norme des filtres.

### Intégration continue

`.github/workflows/tests.yml` compile et lance toute la suite sur Ubuntu 24.04
à chaque push et pull request.

### Lancer les tests sous Linux ou WSL

Dépendances (Ubuntu 24.04) :

```sh
sudo apt install -y build-essential cmake pkg-config libasound2-dev libzmq3-dev cppzmq-dev \
     libconfig++-dev libboost-filesystem-dev python3 python3-zmq
```

Sous WSL, placer le dossier de compilation côté Linux (par exemple `~/sonomkr-build`),
la compilation y est bien plus rapide que sous `/mnt/c`.

```sh
bash tests/run-tests.sh ~/sonomkr-build
# un seul scénario, avec le détail de toutes les bandes :
python3 tests/e2e_wav.py --core ~/sonomkr-build/sonomkr-core --filters conf/filters.conf \
        --scenario multitone_48k -v --keep
```

`--keep` conserve le WAV, la configuration et le journal du cœur dans `/tmp/sonomkr-e2e-*`.
Les tests se désactivent avec `-DSONOMKR_BUILD_TESTS=OFF`.

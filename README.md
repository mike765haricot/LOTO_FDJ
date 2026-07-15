# Essaie Millionaire

Ce projet peut être compilé sur GitHub avec Codespaces ou GitHub Actions.

## Compiler dans GitHub Codespaces

1. Ouvrez ce dépôt dans GitHub Codespaces.
2. Le conteneur C++ se charge automatiquement.
3. Exécutez :

```bash
cmake -S . -B build
cmake --build build
```

L’exécutable sera généré dans le dossier build.

## Compiler via GitHub Actions

Une action de build est déjà configurée dans [.github/workflows/build.yml](.github/workflows/build.yml).

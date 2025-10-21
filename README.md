# 🔐 ZNC FiSH Module

[![License](https://img.shields.io/badge/license-Apache--2.0-blue.svg)](LICENSE)
[![ZNC Version](https://img.shields.io/badge/ZNC-1.11.x-green.svg)](https://znc.in/)
[![OpenSSL](https://img.shields.io/badge/OpenSSL-3-red.svg)](https://www.openssl.org/)

## 📋 Description

Module de chiffrement FiSH pour ZNC, offrant une protection cryptographique des communications IRC via les modes ECB et CBC avec support complet d'OpenSSL 3 et de l'échange de clés DH1080.

### Avantages

- ✅ **Sécurité renforcée** : Chiffrement bout-en-bout des messages IRC (canaux et messages privés)
- ✅ **Compatibilité étendue** : Support ECB (clients legacy) et CBC (standard moderne)
- ✅ **Échange automatique** : Protocol DH1080 pour négociation sécurisée des clés
- ✅ **OpenSSL 3** : Compatible avec les dernières versions de ZNC 1.11.x et OpenSSL 3
- ✅ **Flexibilité** : Configuration par canal, par utilisateur, chiffrement des topics
- ✅ **Interopérabilité** : Compatible avec FiSH/irssi et mIRC FiSH10

---

## 📦 Installation

### Prérequis

- **ZNC** version 1.11.x ou supérieure
- **OpenSSL** version 3.x
- **CMake** version 3.1 ou supérieure
- Compilateur C++ avec support C++11

### Étapes d'installation

1. **Cloner le dépôt**
   ```bash
   git clone https://github.com/ZarTek-Creole/znc-fish.git
   cd znc-fish
   ```

2. **Compiler le module**
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

3. **Installer le module**
   ```bash
   make install
   ```
   Ou copier manuellement `fish.so` dans `~/.znc/modules/`

4. **Charger le module dans ZNC**
   ```irc
   /msg *status LoadMod fish
   ```

5. **Redémarrer ZNC** après remplacement du module pour décharger toute version précédente.

---

## ⚙️ Configuration

### Configuration de base

#### Définir une clé de chiffrement

- **Mode CBC (par défaut, recommandé)** :
  ```irc
  /msg *fish SetKey #canal CBC:maClèSecrète
  ```

- **Mode ECB (pour clients legacy)** :
  ```irc
  /msg *fish SetKey pseudo ECB:cléLegacy
  ```

#### Gestion des clés

- **Lister les clés** :
  ```irc
  /msg *fish ListKeys
  ```
  Ajouter `full` pour afficher les clés complètes : `/msg *fish ListKeys full`

- **Afficher une clé spécifique** :
  ```irc
  /msg *fish ShowKey #canal
  /msg *fish ShowKey pseudo
  ```

- **Supprimer une clé** :
  ```irc
  /msg *fish DelKey #canal
  ```

- **Copier une clé** :
  ```irc
  /msg *fish SetKeyFrom <destination> <source>
  ```

### Échange de clés DH1080

Le protocole DH1080 permet l'échange sécurisé de clés sans transmission directe.

- **Avec un utilisateur** :
  ```irc
  /msg *fish KeyX pseudo [ecb|cbc]
  ```

- **Avec un utilisateur pour un canal** :
  ```irc
  /msg *fish KeyXChan #canal pseudo [ecb|cbc]
  ```

- **Diffusion à tous les utilisateurs d'un canal** :
  ```irc
  /msg *fish KeyXChanAll #canal [ecb|cbc]
  ```

- **Échange automatique** (au premier MP sans clé) :
  ```irc
  /msg *fish AutoKeyX on|off
  ```

Le module accepte `DH1080_INIT` et `DH1080_INIT_CBC`, et envoie/accepte les clés publiques FiSH10 avec suffixe `A`.

### Options avancées

#### Chiffrement des topics

- **Par canal** :
  ```irc
  /msg *fish EncryptTopic #canal on|off|status
  ```

- **Global** :
  ```irc
  /msg *fish EncryptGlobalTopic on|off|status
  ```

#### Désactiver le chiffrement pour une cible

```irc
/msg *fish DisableTarget <#canal|pseudo> on|off
```

#### Préfixe texte clair

Pour envoyer un message en clair malgré une clé active, commencer par `` ou configurer :
```irc
/msg *fish PlainPrefix <préfixe>
```

#### Traitement des messages

- **Messages entrants/sortants** :
  ```irc
  /msg *fish ProcessIncoming on|off
  /msg *fish ProcessOutgoing on|off
  ```

- **Notices et actions** :
  ```irc
  /msg *fish EncryptNotice on|off
  /msg *fish EncryptAction on|off
  ```

#### Marquage des messages (local uniquement)

- **Marquer les messages déchiffrés** :
  ```irc
  /msg *fish MarkIncoming on|off
  /msg *fish MarkIncomingTarget <cible> on|off
  ```

- **Position du marqueur** :
  ```irc
  /msg *fish MarkPos prefix|suffix
  ```

- **Texte du marqueur** :
  ```irc
  /msg *fish MarkStr dec|enc|plain <texte>
  ```

- **Marquer les messages corrompus** :
  ```irc
  /msg *fish MarkBroken on|off
  ```

---

## 🚀 Usage

### Démarrage rapide

1. **Configurer une clé pour un canal** :
   ```irc
   /msg *fish SetKey #moncanal CBC:superSecret123
   ```

2. **Rejoindre le canal et communiquer** :
   - Tous les messages seront automatiquement chiffrés/déchiffrés
   - Les autres utilisateurs doivent avoir la même clé

3. **Vérifier la configuration** :
   ```irc
   /msg *fish ListKeys
   ```

### Modes de chiffrement

#### CBC (Cipher Block Chaining) - Recommandé

- Format : `+OK *<base64>` (IV + texte chiffré, base64 MIME standard)
- Plus sécurisé, standard moderne
- Chaque message a un vecteur d'initialisation unique

#### ECB (Electronic Codebook) - Legacy

- Format : `+OK <fish64>` ou `mcps <fish64>` (base64 FiSH `./0-9a-zA-Z` en blocs de 12 caractères)
- Compatible avec anciens clients FiSH
- Moins sécurisé mais nécessaire pour l'interopérabilité

### Détection et compatibilité

- **Fallback automatique** : Le module essaie d'abord le mode configuré, puis l'autre mode si échec
- **Apprentissage automatique** : Le mode stocké peut être mis à jour lors d'un déchiffrement réussi (sauf si désactivé)
- **Changement de mode** :
  ```irc
  /msg *fish SetMode <cible> ecb|cbc
  /msg *fish GetMode <cible>
  ```

### Stockage des clés

Les clés sont stockées dans ZNC NV avec le format :
- `CBC:<clé>` ou `ECB:<clé>`
- Par défaut CBC si aucun préfixe

### Tests et diagnostics

#### Tests locaux (roundtrip)

```irc
/msg *fish SelfTest ecb MaClé bonjour
/msg *fish SelfTest cbc MaClé bonjour
```

Ces commandes chiffrent puis déchiffrent le texte pour vérifier le bon fonctionnement.

#### Tests d'interopérabilité

Pour tester avec FiSH/irssi ou mIRC FiSH10 :

1. **Messages privés** :
   - Chaque côté : `/msg *fish KeyX <pseudo>`
   - Envoyer un message court comme `!df`

2. **Canal** :
   - Définir la même clé CBC sur tous les clients
   - Vérifier que les messages `+OK *...` sont déchiffrés correctement

---

## 📚 Référence des commandes

### Gestion des clés

- `SetKey <#canal|Pseudo> [CBC:|ECB:]<clé>` — Définir une clé
- `DelKey <#canal|Pseudo>` — Supprimer une clé
- `ShowKey <#canal|Pseudo>` — Afficher une clé
- `ListKeys [full]` — Lister toutes les clés
- `SetKeyFrom <dest> <source>` — Copier une clé
- `SetMode <cible> ecb|cbc` — Définir le mode
- `GetMode <cible>` — Obtenir le mode actuel

### Échange de clés

- `KeyX <pseudo> [ecb|cbc]` — Échanger une clé avec un utilisateur
- `KeyXChan <#canal> <pseudo> [ecb|cbc]` — Échanger pour un canal
- `KeyXChanAll <#canal> [ecb|cbc]` — Diffuser à tous les utilisateurs
- `AutoKeyX on|off` — Échange automatique au premier MP

### Traitement des messages

- `ProcessIncoming on|off` — Traiter les messages entrants
- `ProcessOutgoing on|off` — Traiter les messages sortants
- `EncryptNotice on|off` — Chiffrer les notices
- `EncryptAction on|off` — Chiffrer les actions (/me)

### Topics

- `EncryptTopic <#canal> on|off|status` — Chiffrer le topic d'un canal
- `EncryptGlobalTopic on|off|status` — Paramètre global pour tous les canaux

### Marquage (local)

- `MarkIncoming on|off` — Marquer les messages déchiffrés
- `MarkIncomingTarget <cible> on|off` — Par cible
- `MarkPos prefix|suffix` — Position du marqueur
- `MarkStr dec|enc|plain <texte>` — Texte du marqueur
- `MarkBroken on|off` — Marquer les messages corrompus (ajouter `&` pour blocs FiSH tronqués)

### Utilitaires

- `PlainPrefix <préfixe>` — Définir un préfixe pour texte clair (`` par défaut)
- `DisableTarget <#canal|pseudo> on|off` — Désactiver le chiffrement pour une cible
- `SelfTest <ecb|cbc> <clé> <texte>` — Test de chiffrement roundtrip

---

## 📄 Licence

Ce projet est distribué sous licence **Apache License 2.0**. Consultez le fichier [LICENSE](LICENSE) pour plus de détails.

### Implémentation technique

- L'implémentation ECB respecte le comportement FiSH (clé brute via `BF_set_key(strlen(key), key)` et encodage base64 FiSH)
- Les avertissements de dépréciation OpenSSL 3 sont supprimés dans le code via des pragmas GCC

---

## 🔗 Liens utiles

- [Documentation ZNC](https://wiki.znc.in/)
- [OpenSSL Documentation](https://www.openssl.org/docs/)
- [FiSH Protocol Specification](https://github.com/flakes/mirc_fish_10)
- [Code de Conduite](CODE_OF_CONDUCT.md)
- [Guide de Contribution](CONTRIBUTING.md)
- [Politique de Sécurité](SECURITY.md)
- [Journal des Modifications](CHANGELOG.md)

---

## 💬 Contact & Support

- **Auteur** : [ZarTek-Creole](https://github.com/ZarTek-Creole)
- **Issues** : [Signaler un bug ou demander une fonctionnalité](https://github.com/ZarTek-Creole/znc-fish/issues)
- **Discussions** : [Participer aux discussions](https://github.com/ZarTek-Creole/znc-fish/discussions)
- **Pull Requests** : Les contributions sont les bienvenues ! Consultez [CONTRIBUTING.md](CONTRIBUTING.md)

---

## 🙏 Remerciements

Merci à tous les contributeurs et à la communauté ZNC pour leur support continu.

---

**Note** : Redémarrez ZNC après l'installation ou la mise à jour du module pour garantir le chargement de la version la plus récente.

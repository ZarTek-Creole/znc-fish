# znc-fish
FISH FOR ZNC WITH ECB/CBC SUPPORT AND NEW OPENSSL.
**Résumé :**

Il est nécessaire de réviser le code. Pour commencer, il faut utiliser le module officiel de ZNC pour le cryptage, disponible ici : [ZNC Crypt Module](https://github.com/znc/znc/blob/master/modules/crypt.cpp), qui supporte le mode CBC. Ensuite, il faut intégrer le mode ECB de notre module [ZNC Fish Module](https://github.com/ZarTek-Creole/znc-fish).

**Modifications à apporter au module :**

1. Importer les fonctions du [ZNC Crypt Module](https://github.com/znc/znc/blob/master/modules/crypt.cpp).
2. Chiffrer le fichier de stockage des clés pour améliorer la sécurité des informations sensibles.
3. Mettre en place une commande dédiée pour désactiver temporairement certaines cibles (nicks ou canaux).
4. Désactiver les échanges de clés privées "KeyX" pour éviter les conflits avec fish_10 de mIRC si l'utilisateur les charge.
5. Implémenter une fonctionnalité permettant de préfixer la variable Key avec 'CBC:' ou 'ECB:' suivi de la véritable clé. Par défaut, utiliser CBC si aucun préfixe spécifique n'est indiqué : `SetKey <#canal|Pseudo> [CBC:|ECB:]<vraie_clé>`.

**Bogue connu :**
- Le déchiffrement en mode CBC ne fonctionne pas correctement.

**Sortie Fish :**
```
<utilisateur> Help
<*fish> DelKey <#chan|Nick> : Supprimer une clé pour un nick ou un canal
<*fish> GetSuffixeDecrypt : Obtenir le suffixe pour déchiffrer
<*fish> GetSuffixeEncrypt : Obtenir le suffixe pour chiffrer
<*fish> Help <search> : Générer cette sortie
<*fish> KeyX <Nick> : Commencer un échange de clé avec <Nick>
<*fish> ListKeys : Lister toutes les clés
<*fish> SetKey <#chan|Nick> <Key> : Définir une clé pour un nick ou un canal
<*fish> SetSuffixeDecrypt <char> : Définir le suffixe pour déchiffrer
<*fish> SetSuffixeEncrypt <char> : Définir le suffixe pour chiffrer
<*fish> ShowKey <#chan|Nick> : Montrer la clé de chiffrement de <#chan|Nick>, si elle est définie
<*fish> Version : Afficher la version de ce module
<utilisateur> Version
<*fish> ZNC-FiSH v1.01 par ZarTek-Creole
<*fish> Description : Fish par ZarTek avec support ECB et CBC pour salons et messages privés
<*fish> OpenSSL 3.0.13 30 Jan 2024
<*fish> URL : https://github.com/ZarTek-Creole/znc-fish
```

**Remarque :**

Ce module n'est actuellement PAS destiné à vous protéger contre l'administrateur de votre shell (voir point 1). Les clés sont actuellement stockées en texte clair, donc toute personne ayant accès à votre compte (ou root) peut les obtenir. Il est fortement recommandé d'activer SSL entre ZNC et votre client, sinon le chiffrement s'arrête à ZNC et est envoyé à votre client en texte clair.

**Analyse et suggestions :**

1. **Fusionner les fonctions du [ZNC Crypt Module](https://github.com/znc/znc/blob/master/modules/crypt.cpp)** : Intégrer les fonctions relatives au mode CBC dans le module ZNC Fish après examen du code source du module ZNC Crypt.
2. **Chiffrer le fichier de stockage des clés** : Utiliser une méthode de chiffrement symétrique appropriée, telle que AES, pour chiffrer le fichier contenant les clés avant de le sauvegarder.
3. **Commandes dédiées pour désactiver temporairement des cibles** : Développer deux nouvelles commandes, "DisableChan" pour les canaux et "DisableNick" pour les pseudos, afin de permettre la désactivation temporaire de cibles spécifiques sans supprimer les clés associées.
4. **Élimination des échanges de clés privées "KeyX"** : Bien que bénéfique pour éviter les conflits avec fish_10 de mIRC, il est important d'évaluer les conséquences pour les utilisateurs actuels de cette fonctionnalité. Une alternative serait de permettre la désactivation temporaire via le point 3, bien que cela désactiverait globalement les échanges de clés privées.
5. **Préfixage de la variable Key avec 'CBC:' ou 'ECB:'** : Implémenter cette fonctionnalité pour simplifier la distinction entre les modes CBC et ECB pour chaque clé, offrant ainsi une meilleure flexibilité et clarté.

Concernant le bogue de déchiffrement en mode CBC, il est recommandé de fusionner d'abord les modules pour une meilleure compréhension des interactions avant de résoudre le problème. Cela permettra de diagnostiquer les causes sous-jacentes plus efficacement.

Enfin, il est crucial de rappeler aux utilisateurs que ce module ne protège pas contre l'administrateur du shell. Il est impératif de sécuriser leurs systèmes hôtes et d'activer SSL entre ZNC et le client pour garantir une transmission sécurisée des données. Le chiffrement des clés (voir point 2) apporte une sécurité supplémentaire, mais ne remplace pas une connexion SSL sécurisée.
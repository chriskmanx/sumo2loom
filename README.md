# Sumo2Loom
SUMO (Suggested Upper Merged Ontology) to PowerLoom Translator

# Background

In 2002, a project at the U.S. National Institute of Standards and Technology (NIST) called for the use of an upper ontology in conjunction with an inference engine. At the time, the leading, publicly available candidates for upper ontology were the Suggested Upper Merged Ontology (SUMO) and the OpenCyc ontology [3]. The authoritative version of the former was in a condensed version of the Knowledge Interchange Format (KIF) called SUO-KIF1; the authoritative version of the latter was embedded within the first public release of the OpenCyc inference engine, version 0.6.0b.
After a review of available ontologies and inference engines it was decided to translate SUMO into a form that could be used by version 4.0 of the LOOM inference engine. The translating program Sumo2loom was developed for this purpose.

# Author and Maintenance

The original author is Favid Flater at the U.S. National Institute of Standards and Technology (NIST). The current maintainer if Christoph Kohlhepp. Any derived work is placed under the GNU Public License. Please refer to LICENSE.

# Comments

SUMO has changed format in more than one way since Sumo2Loom was originally written: Both syntax and file formars have evolved. In particular, multi-language support was added while the original Sumo2Loom depended on ASCII characters. I wanted to tackle semantic changes and changes relating to format separetely so notwithstanding the desirability of support for non English users, began by stripping out wide character support. As this is not intended to be permanent, I consigned most of this work into Bash scripts which has allowed me to focus on the smantic aspects of the migration. At present we can convert SUMO into PowerLoom KIF but there are as yet incompatibilities that are being worked in - time permitting.

# Notes

Bash scripts are presently hard-coded to assume directort locations. You may need to modify this to suit your needs.
Most initial changes were made pre-github commit to this project so appear collectively as a batch commmit.


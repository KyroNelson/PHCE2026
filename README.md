# Programming for Hardware Constrained Environments (PHCE)

Welcome to the public Git repository for the course Programming for Hardware Constrained Environments (PHCE), teaching embedded C++ on the Raspberry Pi Pico.

## Getting started

Complete the toolchain setup described in [SETUP.md](SETUP.md) before the first exercise session.

Clone the repository:

```
git clone https://source.coderefinery.org/sdm-edu/phce-2026
```

Exercise code is published on `master`. Create your own branch and do all your work there:

```
git checkout -b <your-branch>
```

For backup and sharing with your exercise partner, create an empty repository on GitHub or GitLab and add it as a second remote:

```
git remote add personal <personal_repo_url>
git push -u personal <your-branch>
```

## Before each exercise session

```
git fetch origin
git checkout master
git pull
git checkout <your-branch>
git merge master
```

If you are new to Git and version control, CodeRefinery has plenty of good teaching materials about [Git and version control](https://coderefinery.github.io/git-intro/), and other related software engineering topics at [CodeRefinery Lessons](https://coderefinery.org/lessons/).

## Repository Settings Already Included

This repository is pre-configured with:

* `.gitignore` tailored to block OS junk (like macOS `._*` or Windows `Thumbs.db`) and local editor states, whilst explicitly allowing collaborative workspace settings.
* `.gitattributes` resolves the Windows (CRLF) vs. macOS/Linux (LF) line-ending conflict by normalizing all Markdown documents to Unix-style line-endings (`LF`) in the Git database automatically.

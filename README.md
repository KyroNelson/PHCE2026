# Embedded C++ using RPi Pico

Welcome to the public Git repository supporting teaching of embedded C++ on the Raspberry Pi Pico.

## Getting started

You can clone or download the code to get started with the programming exercises.
Cloning the repo has the advantage of getting updates to the code by simply "pulling" the repo.

If you are new to Git and version control, CodeRefinery has plenty of good teaching materials about [Git and version control](https://coderefinery.github.io/git-intro/), and other related software engineering topics at [CodeRefinery Lessons](https://coderefinery.org/lessons/).

## Multi-platform Git GUI Clients

Besides the Git command line interface and the Source Control integration in VS Code, there are clients with graphical interface that provide a more visual representation of the source code versioning.
Here is the list of open-source graphical Git clients that support multiple platforms:

- [SourceGit](https://github.com/sourcegit-scm/sourcegit)
- [Gittyup](https://github.com/Murmele/Gittyup)

## IDE Recommendations

While **various IDEs** work perfectly fine with this repository, we have included pre-configured workspace settings for VS Code to optimize your experience, enforce programming learning, and protect your privacy.

### Privacy-First VS Code & VS Codium Configuration

If you use **VS Code** or its fully open-source, telemetry-free counterpart **[VS Codium](https://vscodium.com/)**, this repository includes a pre-configured `.vscode/settings.json` file.

This configuration automatically applies the following workspace-level safeguards:

1. **Disabled Telemetry & Tracking:** Turns off all telemetry levels, diagnostics, and experimental features to Microsoft.
2. **Removed AI Integrations:** Completely disables built-in AI integrations and Microsoft AI features, ensuring an undistracted focus.
3. **Automated Formatting:** Ensures consistent line endings, trailing whitespace cleanup, keeping our Git history clean and readable.

## Toolchain installation

Please follow the course instructions provided in the course materials.

## Repository Settings Already Included

This repository is pre-configured with:

* `.gitignore` tailored to block OS junk (like macOS `._*` or Windows `Thumbs.db`) and local editor states, whilst explicitly allowing collaborative workspace settings.
* `.gitattributes` resolves the Windows (CRLF) vs. macOS/Linux (LF) line-ending conflict by normalizing all Markdown documents to Unix-style line-endings (`LF`) in the Git database automatically.


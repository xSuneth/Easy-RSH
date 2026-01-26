# Git & Versioning Guide

## Version History

### v1.1.0 (Current) - January 27, 2026
**Interactive Setup Wizard Release**

Major features:
- Interactive Vite-like CLI setup wizard
- Arrow-key navigation for selections  
- Animated spinners and loading states
- Configuration persistence
- Custom purple theme RGB(175, 135, 255)
- Enhanced authentication messages

### v1.0.0 - January 26, 2026
**Initial Release**

Core features:
- Remote command execution
- SHA-256 + Salt authentication
- Multi-client fork support
- Session-based authorization

---

## Git Workflow

### View Commit History
```bash
git log --oneline --graph -10
```

### View Tags
```bash
git tag -l -n3
```

### View Changes Since Last Tag
```bash
git log v1.0.0..HEAD --oneline
```

### Create New Version

1. **Update VERSION file**
   ```bash
   echo "1.2.0" > VERSION
   ```

2. **Update CHANGELOG.md**
   - Add new section with version and date
   - List all changes under Added/Changed/Fixed/Removed

3. **Update version in code**
   - Update banner in `src/server/server_main.cpp`
   - Update README if needed

4. **Commit changes**
   ```bash
   git add -A
   git commit -m "chore: Bump version to 1.2.0"
   ```

5. **Create tag**
   ```bash
   git tag -a v1.2.0 -m "Release v1.2.0 - [Brief Description]"
   ```

6. **Push to remote**
   ```bash
   git push origin feature-branch
   git push origin v1.2.0
   ```

---

## Semantic Versioning

This project follows [Semantic Versioning](https://semver.org/):

**Format:** MAJOR.MINOR.PATCH

- **MAJOR** (1.x.x): Breaking changes, incompatible API changes
- **MINOR** (x.1.x): New features, backward compatible
- **PATCH** (x.x.1): Bug fixes, backward compatible

### Examples:
- `1.0.0` → `1.1.0`: Added setup wizard (new feature)
- `1.1.0` → `1.1.1`: Fixed double display bug (bug fix)
- `1.1.0` → `2.0.0`: Changed authentication protocol (breaking change)

---

## Commit Message Convention

Using [Conventional Commits](https://www.conventionalcommits.org/):

### Format:
```
<type>(<scope>): <subject>

<body>

<footer>
```

### Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation only
- `style`: Code style (formatting, semicolons, etc.)
- `refactor`: Code restructuring (no feature/fix)
- `perf`: Performance improvements
- `test`: Adding tests
- `chore`: Build process, dependencies, etc.

### Examples:
```bash
feat(wizard): Add interactive setup wizard

- Implement Config class
- Add arrow-key navigation
- Include animated spinners

Closes #123
```

```bash
fix(ui): Prevent double display in radio buttons

The cursor positioning was incorrect after selection.
```

---

## Current Branch Structure

```
main (production)
  ├─ dev (development)
  └─ feature/new-interactive-setup-v1.1.0 (current)
```

### Workflow:
1. Feature branches → `dev`
2. `dev` (tested) → `main`
3. Tag releases on `main`

---

## Useful Commands

### Check what changed
```bash
git diff HEAD~1              # Changes in last commit
git show HEAD                # Show last commit details
git diff v1.0.0..v1.1.0      # Changes between versions
```

### View file history
```bash
git log --follow -- src/server/Server.cpp
```

### Find when something changed
```bash
git blame src/server/Server.cpp
```

### Revert changes
```bash
git revert HEAD              # Undo last commit (creates new commit)
git reset --soft HEAD~1      # Undo commit, keep changes staged
```

---

## Release Checklist

- [ ] Update VERSION file
- [ ] Update CHANGELOG.md with all changes
- [ ] Update version in code (banners, help text)
- [ ] Update documentation (README, etc.)
- [ ] Run tests
- [ ] Commit all changes
- [ ] Create annotated tag
- [ ] Push commits and tag
- [ ] Create GitHub release (if applicable)

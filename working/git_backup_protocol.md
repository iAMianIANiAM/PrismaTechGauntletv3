# Git Backup Protocol

## Purpose
This document outlines the formal git backup protocol for the PrismaTech Gauntlet 3.0 project. Following this protocol is mandatory to prevent data loss and maintain consistency between working documents and code implementation.

## Core Principles
1. **Simplicity**: Keep commits focused and small
2. **Prudence**: Always verify before committing or pushing
3. **Diligence**: Follow the protocol consistently every time

## When to Commit

Commits should be made at the following points:
1. After implementing a complete, functional component
2. After making significant documentation updates
3. After fixing a bug or issue
4. At logical stopping points in development sessions
5. Before ending a development session

**IMPORTANT**: Never commit code that doesn't build or breaks existing functionality.

## Commit Message Format

All commit messages must follow this format:
```
<type>: <concise description>

<longer description if needed>

<reference to working documents or issues>
```

Types:
- `feat`: New feature or component
- `fix`: Bug fix
- `docs`: Documentation changes
- `test`: Test additions or changes
- `refactor`: Code improvements with no functional changes
- `chore`: Maintenance tasks

### Examples:
```
feat: Implement UltraBasicPositionDetector

- Add unit conversion from raw data to m/s²
- Implement simplified detection with absolute thresholds
- Add position-specific threshold management

References: currentplan_v2.md#L205-L220
```

```
docs: Update directoryIndex with implementation status

- Add [PLANNED] status to missing files
- Update hardware layer verification status
- Add status indicators for all components

References: currentplan_v2.md#L173-L177
```

## Verification Before Commit

Before any commit, perform these verification steps:
1. Ensure the code builds successfully
   ```bash
   pio run -e esp32dev
   ```
2. Verify relevant test environment builds
   ```bash
   # Example for position detection changes
   pio run -e postest
   ```
3. Check that documentation reflects the changes
4. Verify all files referenced in documentation exist
5. Run `git status` to confirm what changes will be committed

## Push Protocol

### When to Push
1. After completing a significant feature
2. At the end of each development session
3. Before making major system changes
4. After synchronizing working documents with code
5. At minimum, once per day when actively developing

### Push Verification Steps
1. Run `git status` to verify local branch status
2. Verify working documents are up to date with code
3. Run `git log origin/recovery-phase1..HEAD` to see unpushed commits
4. Ensure all commits have been properly formatted
5. Execute `git push origin recovery-phase1`
6. Verify push success with `git status`

## Branch Management

### Current Branch Structure
- `recovery-phase1`: Current active development branch for recovery efforts

### Branching Guidelines
1. Use the `recovery-phase1` branch for all current development
2. For significant new features, create feature branches from `recovery-phase1`
3. Name feature branches with descriptive format: `recovery-feature/<name>`
4. Merge completed features back to `recovery-phase1` branch

## End-of-Session Procedure

At the end of each development session:
1. Complete current implementation or reach a logical stopping point
2. Update working documents to reflect current status
3. Commit all changes with proper format
4. Push to remote repository
5. Run `git status` to verify clean state

## Emergency Recovery Procedure

If you encounter repository issues:
1. **DO NOT** force push or reset without consultation
2. Create a backup copy of your working directory
3. Run `git status`, `git log` and document the issue
4. Consult with team before attempting resolution

## Implementation Plan

This protocol is effective immediately for all development work on the PrismaTech Gauntlet 3.0 project. All team members must adhere to these guidelines for all repository operations.

## Future Improvements

Phase 2 of our process improvement plan will include:
1. Creating a commit message template file
2. Implementing a pre-commit hook for file verification
3. Adding repository health monitoring

---

*This protocol was developed as part of the Phase 1 process stabilization efforts and aligns with our project guiding principles of Simplicity, Prudence, and Diligence.* 
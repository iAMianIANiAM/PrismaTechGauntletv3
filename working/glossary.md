# 📘 Project Glossary (Starter)

> This glossary defines key terms, concepts, and protocol references for this project. It will expand as the project progresses. Claude should update this document whenever a new domain-specific term is introduced.

---

## 🔧 Protocols

| Protocol | Description |
|----------|-------------|
| **Analyze** | Root cause investigation protocol for unexpected issues or behavior. Uses a structured four-step approach. |
| **Proposal** | Protocol for suggesting new solutions. Requires user approval before implementation. |
| **Double-Check** | Post-implementation verification protocol. Uses checklists to ensure structural, functional, and build integrity. |
| **Report** | Protocol for information-only responses. No code or action included. |

---

## 🧠 Plan vs. Proposal

| Term | Definition |
|------|------------|
| **Proposal** | A detailed suggestion presented for review. Must not be implemented until explicitly approved. |
| **Plan** | An approved proposal. Must be recorded in `chronicle.md` and is eligible for implementation. |
| **Elevate to Plan** | The act of promoting a proposal to a plan. Claude must confirm elevation before proceeding. |
| **Approval Phrases** | Terms such as "approve," "greenlight," "go ahead," or "make it a plan." Ambiguous language must trigger clarification. |

---

## 🏷️ Chronicle Tags

| Tag | Use |
|-----|-----|
| `🧠 INSIGHT:` | Highlight implementation or design insights. |
| `⚠️ ISSUE:` | Identify bugs, blockers, or risks. |
| `✅ RESOLUTION:` | Document how an issue was resolved. |
| `📌 DECISION:` | Flag finalized design or architectural choices. |
| `🔍 TBD:` | Indicate incomplete or open-ended topics. |

---

## 📁 File References (Initial)

| File | Description |
|------|-------------|
| `chronicle_v#.md` | Running journal of work sessions and insights. Archive at ~1000 lines. |
| `roadmap.md` | Mid-weight directional guide for project goals and milestones. |
| `directoryIndex.md` | Project structure, file purpose mapping, and key relationships. |
| `glossary.md` | This document. Expanded as the project evolves. |

---

## 📊 Implementation Status Tracking

| Status | Definition |
|--------|------------|
| **[PLANNED]** | Designed but not yet implemented. Documentation exists, but code does not. |
| **[IN PROGRESS]** | Implementation started but not complete. Code may exist in partial form. |
| **[IMPLEMENTED]** | Code exists but not fully verified. Builds successfully but not fully tested. |
| **[VERIFIED]** | Implementation exists and has been tested on hardware. Include verification date and method. |
| **[DEPRECATED]** | Was implemented but has been replaced by a newer approach. |

---

## 🔁 Git Workflow Terms

| Term | Definition |
|------|------------|
| **feat** | Commit type for new features or components. |
| **fix** | Commit type for bug fixes. |
| **docs** | Commit type for documentation changes. |
| **test** | Commit type for test additions or changes. |
| **refactor** | Commit type for code improvements with no functional changes. |
| **chore** | Commit type for maintenance tasks. |
| **recovery-phase1** | Current active development branch for recovery efforts. |
| **End-of-Session Procedure** | Set of steps to follow before ending a development session (update docs, commit, push). |

---

## 🧭 Project Guiding Principles

| Principle | Definition |
|-----------|------------|
| **Implementation Before Documentation** | Never document features as complete before they exist in code. |
| **Verification Is Required** | All implementations must be verified before being considered complete. |
| **Small Changes, Frequent Commits** | Prefer small, focused changes with clear documentation. |
| **Recover Before Extending** | Fix and verify existing functionality before adding new features. |
| **Explicit Over Implicit** | Be explicit about status, intentions, and dependencies. |

---

## 🔬 Project-Specific Terms

| Term | Definition |
|------|------------|
| **UBPD** | Ultra Basic Position Detection - The primary detection model for the device. |
| **MPU9250** | The motion sensor used in the gauntlet for position and gesture detection. |
| **Calibration Protocol** | Structured process for capturing sensor data to generate detection thresholds. |
| **Point Detection Model** | Initial implementation of position detection using dominant axis thresholds. |
| **Threshold Manager** | Component for centralizing management of detection thresholds. |

---

> Claude should suggest additions or edits when new terms, acronyms, or conventions arise during development.

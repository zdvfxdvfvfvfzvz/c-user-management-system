# Customer Review Management System
**User Management System in C - Final Project**

## 🎯 Project Overview
A complete review management system featuring **Levenshtein Distance** algorithm for intelligent typo correction, Thai Buddhist Era date support, and comprehensive CRUD operations with undo functionality.

---

## ✨ Key Features

### 1. **Intelligent Typo Correction (Levenshtein Distance Algorithm)**
The system uses edit distance calculation to find reviews even with typos!

**Try These Searches:**
- Search `"jhon"` → Finds "John Doe" ✓
- Search `"sara"` → Finds "Sarah Wilson" ✓
- Search `"poom apirk"` → Finds "Poom Apirak" ✓
- Search `"somcai"` → Finds "Somchai Wong" ✓
- Search `"peter paker"` → Finds "Peter Parker" ✓

**How It Works:**
- Calculates minimum edits needed to transform one string to another
- Distance 0 = Exact match
- Distance 1-2 = Close match (likely typo)
- Distance 3 = Similar name (fuzzy match)
- Uses dynamic programming with 2D matrix (O(m×n) complexity)

### 2. **Thai Buddhist Era (พ.ศ.) Support**
Accepts both Gregorian (CE) and Buddhist Era (BE) dates!

**Valid Date Examples:**
- `2025-12-25` (CE) ✓
- `2568-12-25` (BE = 2025 + 543) ✓
- `2400-2700` (BE range) ✓
- `1900-2100` (CE range) ✓

**Invalid Dates Rejected:**
- `2568-13-01` (month > 12) ✗
- `2025-02-30` (invalid day) ✗
- `9999-99-99` (out of range) ✗

### 3. **Complete CRUD Operations**

#### Create (Add Review)
- Name validation (max 50 chars)
- Score validation (1-5 only)
- Date validation with format checking
- Feedback with length limits
- Dynamic array resizing (capacity doubles when full)

#### Read (Display & Search)
- Display all reviews with pagination
- Search by name (with typo correction)
- Search by score range
- Search by date
- Statistical analysis with bar charts

#### Update
- Update individual fields (name, score, date, feedback)
- Update all fields at once
- Preview before confirming

#### Delete
- Delete by name (with typo matching!)
- Delete by selection from list
- Delete all reviews by a user
- **Double confirmation** (y/n + "DELETE")
- **Undo last delete** feature!

### 4. **Advanced Features**

**Statistics Dashboard:**
# CraveHive Food Ordering System

A food ordering system with a C++ console backend, a browser-based frontend, and a PostgreSQL (Supabase) schema designed for future database migration.

## Overview

This project simulates an end-to-end food ordering workflow — user registration/login, browsing restaurants and menus, cart management, order checkout, and an admin panel to process orders.

## Components

### 1. C++ Backend (`FoodSystem.cpp`)
- Built using Object-Oriented Programming principles: a base `Entity` class, with `User`, `Hotel`, and `FoodItem` derived from it, and `FoodSystem` inheriting from `UserManagement`.
- Implements a **custom circular queue** (`OrderQueue`) using a fixed-size array for order processing (`enqueue`, `dequeue`, `peek`, `displayAll`).
- Restaurant and menu data loaded from `hotels.txt`; user data persisted to `users.txt`.
- Features: user registration/login, admin login, browsing restaurants by category (Vegetarian / Non-Vegetarian / Beverages), cart operations, checkout, and an admin panel to view and process queued orders.

### 2. Frontend (`index.html`)
- HTML/CSS/JavaScript single-page interface mirroring the backend workflow.
- Screens: welcome/auth, restaurant listing, menu browsing, cart, order confirmation, admin panel, and user order history.
- Cart and order queue are managed client-side in memory (JavaScript arrays) — not yet connected to a persistent database.

### 3. Database Schema (Supabase migration)
- A normalized PostgreSQL schema designed for Supabase, covering `profiles`, `restaurants`, `menu_items`, `orders`, and `order_items`.
- Includes foreign key relationships, indexes, and Row-Level Security (RLS) policies for user-scoped data access.
- Located in `supabase/migrations/`.

## Project Status

- C++ backend and HTML/CSS/JS frontend are functional but run independently, each with their own in-memory/file-based state.
- The Supabase schema has been designed as a migration path to move the frontend to a persistent, multi-user database — integration is in progress.

## Tech Stack

- **Backend:** C++ (OOP, custom queue/array-based data structures)
- **Frontend:** HTML, CSS, JavaScript
- **Database:** PostgreSQL via Supabase (schema + RLS policies)

## Setup

### Backend
```bash
g++ FoodSystem.cpp -o FoodSystem
./FoodSystem
```

### Frontend
Open `index.html` directly in a browser.

### Database
```bash
supabase login
supabase link --project-ref YOUR_PROJECT_REF
supabase db push
```

## Future Work
- Connect frontend to Supabase (replace in-memory state with live queries).
- Migrate C++ backend's file-based persistence (`users.txt`, `hotels.txt`) to the Supabase database.
- Add authentication via Supabase Auth instead of custom username/password handling.

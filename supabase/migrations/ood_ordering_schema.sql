-- ============================================
-- FOOD ORDERING SYSTEM — SUPABASE MIGRATION
-- ============================================

-- 1. PROFILES (extends Supabase auth.users)
create table profiles (
  id uuid primary key references auth.users(id) on delete cascade,
  username text unique not null,
  is_admin boolean default false,
  created_at timestamptz default now()
);

-- 2. RESTAURANTS (hotels)
create table restaurants (
  id uuid primary key default gen_random_uuid(),
  name text not null unique,
  created_at timestamptz default now()
);

-- 3. MENU ITEMS
create table menu_items (
  id uuid primary key default gen_random_uuid(),
  restaurant_id uuid not null references restaurants(id) on delete cascade,
  category text not null check (category in ('veg', 'non-veg', 'beverage')),
  name text not null,
  price numeric(10,2) not null check (price >= 0),
  created_at timestamptz default now()
);

-- 4. ORDERS
create table orders (
  id uuid primary key default gen_random_uuid(),
  user_id uuid not null references profiles(id) on delete cascade,
  restaurant_id uuid not null references restaurants(id),
  status text not null default 'pending' check (status in ('pending', 'processing', 'completed', 'cancelled')),
  total numeric(10,2) not null default 0,
  created_at timestamptz default now()
);

-- 5. ORDER ITEMS (snapshot of price/name at order time — price change aana affect aagadhu)
create table order_items (
  id uuid primary key default gen_random_uuid(),
  order_id uuid not null references orders(id) on delete cascade,
  menu_item_id uuid references menu_items(id) on delete set null,
  item_name text not null,
  item_price numeric(10,2) not null,
  quantity int not null check (quantity > 0)
);

-- ============================================
-- INDEXES (fast lookups)
-- ============================================
create index idx_menu_items_restaurant on menu_items(restaurant_id);
create index idx_orders_user on orders(user_id);
create index idx_orders_status on orders(status);
create index idx_order_items_order on order_items(order_id);

-- ============================================
-- ROW LEVEL SECURITY (RLS)
-- ============================================
alter table profiles enable row level security;
alter table restaurants enable row level security;
alter table menu_items enable row level security;
alter table orders enable row level security;
alter table order_items enable row level security;

-- Profiles: user can see/edit own, admin can see all
create policy "Users view own profile" on profiles
  for select using (auth.uid() = id);
create policy "Users update own profile" on profiles
  for update using (auth.uid() = id);

-- Restaurants & Menu items: public read (anyone browsing menu)
create policy "Anyone can view restaurants" on restaurants
  for select using (true);
create policy "Anyone can view menu items" on menu_items
  for select using (true);

-- Orders: users see only their own orders, admins see all
create policy "Users view own orders" on orders
  for select using (
    auth.uid() = user_id
    or exists (select 1 from profiles where id = auth.uid() and is_admin = true)
  );
create policy "Users create own orders" on orders
  for insert with check (auth.uid() = user_id);
create policy "Admins update orders" on orders
  for update using (
    exists (select 1 from profiles where id = auth.uid() and is_admin = true)
  );

-- Order items: same visibility as parent order
create policy "Users view own order items" on order_items
  for select using (
    exists (
      select 1 from orders
      where orders.id = order_items.order_id
      and (orders.user_id = auth.uid()
           or exists (select 1 from profiles where id = auth.uid() and is_admin = true))
    )
  );
create policy "Users insert own order items" on order_items
  for insert with check (
    exists (select 1 from orders where orders.id = order_id and orders.user_id = auth.uid())
  );

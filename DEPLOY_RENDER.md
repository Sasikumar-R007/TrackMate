# Deploy TrackMate to Render - Step by Step Guide

## 🚀 Quick Deployment Steps

### Step 1: Push to GitHub (if not already done)

1. Go to [GitHub.com](https://github.com) and create a new repository
2. Name it: `trackmate-app` (or any name you like)
3. **Important:** Make it PUBLIC (or you'll need Render paid plan)
4. Don't initialize with README (we already have files)

Then run these commands in Replit Shell:
```bash
git init
git add .
git commit -m "Initial commit for Render deployment"
git branch -M main
git remote add origin https://github.com/YOUR_USERNAME/trackmate-app.git
git push -u origin main
```

Replace `YOUR_USERNAME` with your GitHub username!

### Step 2: Deploy on Render

1. **Go to [Render.com](https://render.com)** and sign up (free!)

2. **Click "New +"** → Select **"Web Service"**

3. **Connect your GitHub repository:**
   - Click "Connect account" to link GitHub
   - Select your `trackmate-app` repository

4. **Configure the deployment:**
   - **Name:** `trackmate` (or your choice)
   - **Region:** Choose closest to you
   - **Branch:** `main`
   - **Runtime:** `Python 3`
   - **Build Command:** `pip install -r requirements.txt`
   - **Start Command:** `gunicorn --bind 0.0.0.0:$PORT main:app`

5. **Set Environment Variables:**
   - Click "Advanced" 
   - Add variable:
     - Key: `SESSION_SECRET`
     - Value: `your-random-secret-key-12345` (or any random string)

6. **Select Free Plan** (at the bottom)

7. **Click "Create Web Service"**

### Step 3: Wait for Deployment

- Render will build and deploy your app (takes 2-5 minutes)
- You'll see logs showing the build process
- Once complete, you'll get a URL like: `https://trackmate.onrender.com`

### Step 4: Access Your App

- Click the URL provided by Render
- Your TrackMate app is now LIVE! 🎉

---

## 📝 Important Notes

### Free Tier Limitations:
- Your app will "sleep" after 15 minutes of inactivity
- First visit after sleep takes ~30 seconds to wake up
- Perfect for demonstrations and testing!

### Database:
- Currently using SQLite (stored in project)
- Data persists as long as app is running
- For permanent storage, upgrade to PostgreSQL (also free on Render)

### If You Get Stuck:
1. Check Render logs for errors
2. Make sure all files are pushed to GitHub
3. Verify the start command is correct
4. Ensure PORT is used (already configured!)

---

## 🔧 Optional: Add PostgreSQL (Free)

If you want persistent database:

1. In Render Dashboard → Click "New +" → "PostgreSQL"
2. Name it `trackmate-db` → Create
3. Copy the "Internal Database URL"
4. Go to your Web Service → Environment
5. Add: `DATABASE_URL` = (paste the URL)
6. I can help migrate the code to use PostgreSQL if needed!

---

## ✅ Quick Checklist

- [ ] Push code to GitHub (public repo)
- [ ] Create Render account
- [ ] Connect GitHub to Render
- [ ] Configure Web Service settings
- [ ] Add SESSION_SECRET env variable
- [ ] Deploy and wait
- [ ] Visit your live URL!

**Your app will be live in under 10 minutes!** 🚀

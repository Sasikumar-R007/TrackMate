# 🚀 Render Deployment Guide for TrackMate

## Complete Setup Instructions for Render.com

---

## 📋 Prerequisites

✅ Your TrackMate project is deployed on Render  
✅ You have access to the Render dashboard  
✅ You have the HARDWARE_API_KEY value from Replit

---

## 🔑 Step 1: Add Environment Variable on Render

Your ESP32 hardware needs the **HARDWARE_API_KEY** to communicate securely with your Render app.

### Method 1: Using Render Dashboard (Recommended)

1. **Go to Render Dashboard**
   - Visit: https://dashboard.render.com
   - Find your TrackMate web service

2. **Navigate to Environment Variables**
   - Click on your service name
   - Click **"Environment"** tab in the left sidebar
   - You'll see the "Environment Variables" section

3. **Add the HARDWARE_API_KEY**
   - Click **"Add Environment Variable"** button
   - **Key**: `HARDWARE_API_KEY`
   - **Value**: Paste your API key from Replit Secrets
   - Click **"Save Changes"**

4. **Wait for Automatic Redeploy**
   - Render will automatically redeploy your service (~2-3 minutes)
   - You'll see "Deploy in progress..." message
   - Wait for "Live" status

### Method 2: Using Render Blueprint (render.yaml)

If you have a `render.yaml` file in your repository:

```yaml
services:
  - type: web
    name: trackmate
    env: python
    buildCommand: pip install -r requirements.txt
    startCommand: gunicorn --bind 0.0.0.0:5000 main:app
    envVars:
      - key: HARDWARE_API_KEY
        sync: false  # Will prompt for value during deployment
```

Push this to your Git repo and Render will prompt you for the API key value.

---

## 📍 Step 2: Get Your Render URL

1. **Find Your Service URL**
   - In Render Dashboard → Your service
   - Look for the URL at the top (e.g., `https://trackmate-abc123.onrender.com`)
   - Copy this URL

2. **Your API Endpoint**
   - Full endpoint: `https://YOUR-APP-NAME.onrender.com/api/esp32/scan`
   - Example: `https://trackmate-abc123.onrender.com/api/esp32/scan`

---

## 🔧 Step 3: Update ESP32 Code for Render

You need to update your ESP32 Arduino code with:
1. Your Render URL
2. Your HARDWARE_API_KEY

### Update These Lines in Your .ino File:

```cpp
// Replace with your Render URL
const char* serverURL = "https://YOUR-APP-NAME.onrender.com/api/esp32/scan";

// Replace with your HARDWARE_API_KEY from Replit
const char* apiKey = "YOUR_API_KEY_HERE";
```

**Example:**
```cpp
const char* serverURL = "https://trackmate-abc123.onrender.com/api/esp32/scan";
const char* apiKey = "a7f3d9e2b4c8f1a5d8e9c7b6a4f2d1e8";
```

---

## ✅ Step 4: Verify Setup

### Test API Endpoint

Use this curl command to test (replace with your values):

```bash
curl -X POST https://YOUR-APP-NAME.onrender.com/api/esp32/scan \
  -H "Content-Type: application/json" \
  -H "X-API-Key: YOUR_API_KEY" \
  -d '{
    "fingerprint_id": 1,
    "bus_number": "Bus-1",
    "scan_type": "entry"
  }'
```

**Expected Response:**
```json
{
  "success": false,
  "message": "Student not found"
}
```
(This is normal - means API is working, just no student with that fingerprint ID yet)

**If you get authentication error:**
```json
{
  "success": false,
  "message": "Invalid or missing API key"
}
```
→ Check that HARDWARE_API_KEY matches exactly on both Render and ESP32 code

---

## 🌐 Step 5: Important Render-Specific Notes

### Free Tier Sleep Mode
⚠️ **Important**: Render free tier services sleep after 15 minutes of inactivity

**Solutions:**
1. **Upgrade to paid plan** ($7/month) - Recommended for production
2. **Use a ping service** - Keep your app awake
   - UptimeRobot: https://uptimerobot.com (free)
   - Cron-job.org: https://cron-job.org (free)
   - Set to ping your URL every 10 minutes

3. **Wake-up delay**: If app sleeps, first request after waking takes ~30 seconds
   - ESP32 will show timeout error
   - Second scan will work normally

### Database on Render

If you're using PostgreSQL on Render:

1. **Create PostgreSQL Database**
   - Dashboard → "New +" → "PostgreSQL"
   - Copy the "Internal Database URL"

2. **Add DATABASE_URL Environment Variable**
   - Key: `DATABASE_URL`
   - Value: Your PostgreSQL connection string
   - **Important**: Change `postgres://` to `postgresql://`

3. **Update requirements.txt**
   ```
   psycopg2-binary==2.9.11
   ```

### HTTPS Requirements

✅ **Good News**: Render provides free SSL/HTTPS automatically
- All URLs are `https://` by default
- ESP32 code works with HTTPS (no certificate validation needed)
- Your data is encrypted in transit

---

## 📱 Quick Checklist for Render Deployment

- [ ] Service deployed and showing "Live" status
- [ ] HARDWARE_API_KEY added to Environment Variables
- [ ] Service redeployed after adding environment variable
- [ ] Render URL copied (e.g., https://trackmate-abc123.onrender.com)
- [ ] ESP32 code updated with Render URL
- [ ] ESP32 code updated with HARDWARE_API_KEY
- [ ] API endpoint tested with curl (returns valid JSON)
- [ ] If free tier: ping service configured to prevent sleep
- [ ] Database connected (if using PostgreSQL)

---

## 🔄 Updating Environment Variables Later

**To change HARDWARE_API_KEY:**

1. Generate new API key (if needed)
2. Update on Render:
   - Dashboard → Service → Environment
   - Click the edit icon next to HARDWARE_API_KEY
   - Paste new value → Save Changes
3. Update ESP32 code with new key
4. Re-upload to ESP32

**Important**: Both must match exactly!

---

## 🐛 Troubleshooting Render Issues

### Service Won't Start
**Check logs:**
- Dashboard → Your service → "Logs" tab
- Look for Python errors or missing dependencies

**Common fixes:**
- Ensure `requirements.txt` has all dependencies
- Check `gunicorn` is in requirements.txt
- Verify start command: `gunicorn --bind 0.0.0.0:5000 main:app`

### ESP32 Can't Connect
**Error: "Server Error! Code: -1"**
- Render app might be sleeping (free tier)
- Wait 30 seconds and try again
- Set up ping service to keep awake

**Error: "401 Unauthorized"**
- API key mismatch
- Verify HARDWARE_API_KEY on Render matches ESP32 code exactly

**Error: "404 Not Found"**
- Wrong URL in ESP32 code
- Should end with `/api/esp32/scan`
- Example: `https://trackmate-abc123.onrender.com/api/esp32/scan`

### Database Connection Fails
**"database connection error"**
- Check DATABASE_URL is set correctly
- Ensure using `postgresql://` not `postgres://`
- Verify PostgreSQL database is created and running

---

## 🎯 Render vs Replit: Key Differences

| Feature | Replit | Render |
|---------|--------|--------|
| **URL Format** | `*.replit.dev` | `*.onrender.com` |
| **Environment Vars** | Replit Secrets | Render Environment |
| **Sleep Mode** | Always awake (Autoscale) | Free tier sleeps after 15min |
| **Database** | Neon PostgreSQL | Render PostgreSQL or external |
| **Deployment** | Auto on file change | Git push or manual deploy |
| **Cost** | Credits-based | Free tier + paid plans |

**For ESP32 Integration:**
- Update `serverURL` to your Render URL
- Add `HARDWARE_API_KEY` to Render Environment Variables
- Everything else works the same!

---

## ✅ Final Verification Steps

1. **Check Render Dashboard**
   ```
   ✓ Service status: Live (green)
   ✓ Environment: HARDWARE_API_KEY is set
   ✓ Logs: No errors, server started successfully
   ```

2. **Test Web App**
   ```
   ✓ Open your Render URL in browser
   ✓ Login page loads correctly
   ✓ Can register and login as teacher/parent
   ```

3. **Test API Endpoint**
   ```
   ✓ curl command returns valid JSON
   ✓ API key is accepted (no 401 errors)
   ✓ Endpoint returns error for non-existent student (expected)
   ```

4. **Test ESP32**
   ```
   ✓ Upload code → Serial Monitor shows "WiFi Connected"
   ✓ Scan fingerprint → Shows student name (if enrolled)
   ✓ Check web dashboard → Attendance recorded
   ```

---

## 🎉 You're Done!

Your TrackMate system is now deployed on Render and ready for ESP32 integration!

**Support Resources:**
- Render Docs: https://render.com/docs
- Render Status: https://status.render.com
- Community: https://community.render.com

---

## 🔗 Important URLs Summary

**Your Render Service:**
- Dashboard: https://dashboard.render.com
- Your App: `https://YOUR-APP-NAME.onrender.com`
- API Endpoint: `https://YOUR-APP-NAME.onrender.com/api/esp32/scan`

**Environment Variables:**
- Location: Service → Environment tab
- Required: `HARDWARE_API_KEY` (must match ESP32 code)
- Optional: `DATABASE_URL` (if using PostgreSQL)

**ESP32 Configuration:**
```cpp
const char* serverURL = "https://YOUR-APP-NAME.onrender.com/api/esp32/scan";
const char* apiKey = "YOUR_HARDWARE_API_KEY_HERE";
```

Happy Deploying! 🚀

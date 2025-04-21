
# API Documentation

This REST API provides authenticated users the ability to manage **routes** and **route points**.
You can create, view, and delete routes, as well as add, retrieve, or delete individual points.

> All endpoints require Token Authentication.  
> Pass your token in the `Authorization` header:
> ```
Authorization: Token your_token_here
```

---

## Routes

### `GET /api/routes/`

**Description:** Get a list of all routes for the authenticated user.

**Response:**
```json
[
  {
    "id": 1,
    "name": "My First Route",
    "background": 2,
    "user": 5,
    "points": [
      {
        "id": 10,
        "name": "Point A",
        "x": 100,
        "y": 150,
        "route": 1
      }
    ]
  }
]
```

---

###  `POST /api/routes/`

**Description:** Create a new route.

**Request Body:**
```json
{
  "name": "New Route",
  "background": 1
}
```

**Response:**
```json
{
  "id": 2,
  "name": "New Route",
  "background": 1,
  "user": 5,
  "points": []
}
```

---

###  `GET /api/routes/{route_id}/`

**Description:** Retrieve details for a specific route.

**Response:**
```json
{
  "id": 1,
  "name": "My First Route",
  "background": 2,
  "user": 5,
  "points": [
    {
      "id": 10,
      "name": "Point A",
      "x": 100,
      "y": 150,
      "route": 1
    }
  ]
}
```

---

###  `DELETE /api/routes/{route_id}/`

**Description:** Delete the specified route.

**Response:**  
HTTP 204 No Content

---

##  Route Points

###  `GET /api/routes/{route_id}/points/`

**Description:** Retrieve all points for the specified route.

**Response:**
```json
[
  {
    "id": 10,
    "name": "Point A",
    "x": 100,
    "y": 150,
    "route": 1
  },
  {
    "id": 11,
    "name": "Point B",
    "x": 200,
    "y": 300,
    "route": 1
  }
]
```

---

###  `POST /api/routes/{route_id}/points/`

**Description:** Add a new point to the specified route.

**Request Body:**
```json
{
  "name": "Point C",
  "x": 250,
  "y": 220
}
```

**Response:**
```json
{
  "id": 12,
  "name": "Point C",
  "x": 250,
  "y": 220,
  "route": 1
}
```

---

###  `GET /api/routes/{route_id}/points/{point_id}/`

**Description:** Retrieve details of a specific point.

**Response:**
```json
{
  "id": 10,
  "name": "Point A",
  "x": 100,
  "y": 150,
  "route": 1
}
```

---

###  `DELETE /api/routes/{route_id}/points/{point_id}/`

**Description:** Delete a specific point.

**Response:**  
HTTP 204 No Content

---

##  Authentication Example (curl)

```bash
curl -H "Authorization: Token your_token_here" http://localhost:8000/api/routes/
```

---

##  Notes

- All `POST` requests must include a valid CSRF token if using session authentication.
- All endpoints are protected — only authenticated users can view and modify their own data.
  
---

📚 Built with Django REST Framework.

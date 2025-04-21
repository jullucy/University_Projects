from django.contrib.auth.models import User
from rest_framework.test import APITestCase
from rest_framework.authtoken.models import Token
from rest_framework import status
from routes.models import BackgroundImage, Route, RoutePoint


class RouteAPITestCase(APITestCase):
    def setUp(self):
        self.user = User.objects.create_user(username="tester", password="pass")
        self.token = Token.objects.create(user=self.user)
        self.client.credentials(HTTP_AUTHORIZATION="Token " + self.token.key)

        self.background = BackgroundImage.objects.create(name="Test BG", image="media/backgrounds/bialoleka_zcBtpAJ.jpg")
        self.route = Route.objects.create(name="Test route", user=self.user, background=self.background)

    def test_get_route_list(self):
        response = self.client.get("/routes/api/routes/")
        self.assertEqual(response.status_code, status.HTTP_200_OK)
        self.assertEqual(len(response.data), 1)
        self.assertEqual(response.data[0]["name"], "Test route")

    def test_create_route(self):
        response = self.client.post("/routes/api/routes/", {
            "name": "New route",
            "background": self.background.id
        })
        self.assertEqual(response.status_code, status.HTTP_201_CREATED)
        self.assertEqual(Route.objects.count(), 2)
        self.assertEqual(response.data["name"], "New route")

    def test_get_route_detail(self):
        response = self.client.get(f"/routes/api/routes/{self.route.id}/")
        self.assertEqual(response.status_code, status.HTTP_200_OK)
        self.assertEqual(response.data["name"], "Test route")

    def test_delete_route(self):
        response = self.client.delete(f"/routes/api/routes/{self.route.id}/")
        self.assertEqual(response.status_code, status.HTTP_204_NO_CONTENT)
        self.assertEqual(Route.objects.count(), 0)

    def test_unauthorized_access(self):
        self.client.credentials()  # Remove auth
        response = self.client.get("/routes/api/routes/")
        self.assertEqual(response.status_code, status.HTTP_401_UNAUTHORIZED)


class RoutePointAPITestCase(APITestCase):
    def setUp(self):
        self.user = User.objects.create_user(username="tester", password="pass")
        self.token = Token.objects.create(user=self.user)
        self.client.credentials(HTTP_AUTHORIZATION="Token " + self.token.key)

        self.background = BackgroundImage.objects.create(name="BG", image="media/backgrounds/brodno_targowek_T9xMsbP.jpg")
        self.route = Route.objects.create(name="Test Route", user=self.user, background=self.background)

    def test_add_point_to_route(self):
        url = f"/routes/api/routes/{self.route.id}/points/"
        response = self.client.post(url, {
            "name": "Punkt A",
            "x": 100,
            "y": 200
        })
        self.assertEqual(response.status_code, status.HTTP_201_CREATED)
        self.assertEqual(RoutePoint.objects.count(), 1)
        self.assertEqual(response.data["name"], "Punkt A")

    def test_get_points_for_route(self):
        point = RoutePoint.objects.create(name="Point 1", x=50, y=60, user=self.user, route=self.route)
        response = self.client.get(f"/routes/api/routes/{self.route.id}/points/")
        self.assertEqual(response.status_code, 200)
        self.assertEqual(len(response.data), 1)
        self.assertEqual(response.data[0]["name"], "Point 1")

    def test_delete_point(self):
        point = RoutePoint.objects.create(name="To delete", x=10, y=20, user=self.user, route=self.route)
        response = self.client.delete(f"/routes/api/routes/{self.route.id}/points/{point.id}/")
        self.assertEqual(response.status_code, 204)
        self.assertEqual(RoutePoint.objects.count(), 0)

    def test_update_point(self):
        point = RoutePoint.objects.create(name="Old name", x=10, y=20, user=self.user, route=self.route)
        response = self.client.put(f"/routes/api/routes/{self.route.id}/points/{point.id}/", {
            "name": "Updated name",
            "x": 15,
            "y": 25,
            "route": self.route.id
        })
        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.data["name"], "Updated name")

    def test_unauthorized_point_access(self):
        point = RoutePoint.objects.create(name="Secret Point", x=10, y=20, user=self.user, route=self.route)
        self.client.credentials()  # Remove auth
        response = self.client.get(f"/routes/api/routes/{self.route.id}/points/{point.id}/")
        self.assertEqual(response.status_code, 401)

    def test_point_list_only_for_authenticated_user(self):
        other_user = User.objects.create_user(username="hacker", password="pass")
        other_route = Route.objects.create(name="Hacker's Route", user=other_user, background=self.background)
        RoutePoint.objects.create(name="Other Point", x=0, y=0, user=other_user, route=other_route)

        my_point = RoutePoint.objects.create(name="My Point", x=1, y=1, user=self.user, route=self.route)

        response = self.client.get(f"/routes/api/routes/{self.route.id}/points/")
        self.assertEqual(len(response.data), 1)
        self.assertEqual(response.data[0]["name"], "My Point")
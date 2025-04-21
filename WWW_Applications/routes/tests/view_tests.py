from django.test import TestCase
from django.urls import reverse
from django.contrib.auth.models import User
from routes.models import BackgroundImage, Route, RoutePoint

class WebViewTests(TestCase):
    def setUp(self):
        self.user = User.objects.create_user(username='testuser', password='pass123')
        self.background = BackgroundImage.objects.create(name="BG", image="backgrounds/bg.jpg")
        self.client.login(username='testuser', password='pass123')

    def test_create_route_view_get(self):
        response = self.client.get(reverse('create_route'))
        self.assertEqual(response.status_code, 200)
        self.assertContains(response, "backgrounds/bg.jpg")

    def test_create_route_view_post(self):
        response = self.client.post(reverse('create_route'), {
            "name": "Test Route",
            "background": self.background.id
        })
        self.assertEqual(response.status_code, 302)
        self.assertEqual(Route.objects.count(), 1)

    def test_my_routes_view(self):
        Route.objects.create(name="Route 1", background=self.background, user=self.user)
        response = self.client.get(reverse('my_routes'))
        self.assertEqual(response.status_code, 200)
        self.assertContains(response, "Route 1")

    def test_edit_route_view_get(self):
        route = Route.objects.create(name="Route 1", background=self.background, user=self.user)
        response = self.client.get(reverse('edit_route', args=[route.id]))
        self.assertEqual(response.status_code, 200)
        self.assertContains(response, "Route 1")

    def test_edit_route_view_post_add_point(self):
        route = Route.objects.create(name="Route with Points", background=self.background, user=self.user)
        response = self.client.post(
            reverse('edit_route', args=[route.id]),
            data='{"x": 100, "y": 200}',
            content_type='application/json'
        )
        self.assertEqual(response.status_code, 200)
        self.assertEqual(RoutePoint.objects.count(), 1)

    def test_remove_route_view(self):
        route = Route.objects.create(name="Removable", background=self.background, user=self.user)
        response = self.client.get(reverse('remove_route', args=[route.id]))
        self.assertEqual(response.status_code, 302)
        self.assertEqual(Route.objects.count(), 0)

from django.urls import path, include
from .views import register, login_view, logout_view, create_route, my_routes, edit_route, remove_route, remove_point, add_point

from django.conf import settings
from django.conf.urls.static import static
from .views import home

from rest_framework.routers import DefaultRouter
from rest_framework_nested.routers import NestedDefaultRouter
from rest_framework.authtoken.views import obtain_auth_token
from .views import RouteViewSet, RoutePointViewSet


router = DefaultRouter()
router.register(r'routes', RouteViewSet, basename='routes')
routes_router = NestedDefaultRouter(router, r'routes', lookup='route')
routes_router.register(r'points', RoutePointViewSet, basename='route-points')

urlpatterns = [
    path("", home, name="home"),
    path("register/", register, name="register"),
    path("login/", login_view, name="login"),
    path("logout/", logout_view, name="logout"),
    path('create_route/', create_route, name='create_route'),
    path('my_routes/', my_routes, name='my_routes'),
    path('edit_route/<int:route_id>/', edit_route, name='edit_route'),
    path('add_point/<int:route_id>/', add_point, name='add_point'),
    path('remove_route/<int:route_id>/', remove_route, name='remove_route'),
    path('api/', include(router.urls)),
    path('api/', include(routes_router.urls)),
    path('api/token-auth/', obtain_auth_token),
    path('remove_point/<int:point_id>/', remove_point, name='remove_point'),
]

urlpatterns += static(settings.MEDIA_URL, document_root=settings.MEDIA_ROOT)
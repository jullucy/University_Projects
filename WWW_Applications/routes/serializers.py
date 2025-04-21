from rest_framework import serializers
from .models import Route, RoutePoint

class RoutePointSerializer(serializers.ModelSerializer):
    class Meta:
        model = RoutePoint
        fields = ['id', 'name', 'x', 'y', 'route']
        extra_kwargs = {
            'route': {'read_only': True},
        }

class RouteSerializer(serializers.ModelSerializer):
    points = RoutePointSerializer(many=True, read_only=True)

    class Meta:
        model = Route
        fields = ['id', 'name', 'background', 'user', 'points']
        extra_kwargs = {
            'user': {'read_only': True},
        }
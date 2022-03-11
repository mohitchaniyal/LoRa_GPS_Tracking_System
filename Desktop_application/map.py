from kivymd.app import MDApp
from kivy_garden.mapview import MapView , MapMarker,MapSource


class MapViewApp(MDApp):
	def build(self):
		print(MapSource.providers.keys)
		mapview = MapView(zoom=30, lat=27, lon=78)
		mapview.map_source="osm"
		marker=MapMarker()
		marker.lat=27
		marker.lon=78
		mapview.add_widget(marker)
		return mapview

MapViewApp().run()
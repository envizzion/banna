
# Importing required libraries 
from googleplaces import GooglePlaces, types, lang ,ranking
import requests 
import json 
  
# This is the way to make api requests 
# using python requests library 
  
# send_url = 'http://freegeoip.net/json' 
# r = requests.get(send_url) 
# j = json.loads(r.text) 
# print(j) 
# lat = j['latitude'] 
# lon = j['longitude'] 
  
# Generate an API key by going to this location 
# https://cloud.google.com /maps-platform/places/?apis = 
# places in the google developers 
  
# Use your own API key for making api request calls 
API_KEY = 'AIzaSyDn6ibMkPwRUaMBJwnTLfTyozRCeUaDTUk'
  
# Initialising the GooglePlaces constructor 
google_places = GooglePlaces(API_KEY) 
  
# call the function nearby search with 
# the parameters as longitude, latitude, 
# radius and type of place which needs to be searched of  
# type can be HOSPITAL, CAFE, BAR, CASINO, etc 
query_result = google_places.nearby_search( 
        # lat_lng ={'lat': 46.1667, 'lng': -1.15}, 
        lat_lng ={'lat': 7.092950, 'lng': 80.002524}, 
        # radius = 1000, 
        
        
        # keyword='general',
        # types =[types.TYPE_HOSPITAL] or 
        # [types.TYPE_CAFE] or [type.TYPE_BAR] 
        # or [type.TYPE_CASINO]) 
        type =types.TYPE_POLICE) 
  
# If any attributions related  
# with search results print them 
if query_result.has_attributions: 
    print (query_result.html_attributions) 
  
placeID = ""  
# Iterate over the search results 
for place in query_result.places: 
    # print(type(place)) 
    # place.get_details() 
    print (place.name) 
    print("Latitude", place.geo_location['lat']) 
    print("Longitude", place.geo_location['lng']) 
    placeID = place.place_id
    print("id:",placeID)
    # place.get_details()
    # print(place)
    print() 
    break


query_result2 = google_places.get_place(

   place_id = placeID,

 ).get_details()

if query_result2.has_attributions: 
    print (query_result2.html_attributions) 
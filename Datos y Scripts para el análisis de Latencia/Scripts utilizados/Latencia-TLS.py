import pandas as pd
from datetime import datetime

# Cargar los archivos CSV (emisor y receptor)
ArchivoEmisor = pd.read_csv('Emisor-TLS.csv')  # Ruta del archivo CSV del emisor sin cifrado
ArchivoReceptor = pd.read_csv('Receptor-TLS.csv')  # Ruta del archivo CSV del receptor sin cifrado

# Unificación de las columnas "Fecha" y "Hora" para crear un "timestamp" único
ArchivoEmisor['timestamp'] = pd.to_datetime(ArchivoEmisor['Fecha'] + ' ' + ArchivoEmisor['Hora'])
ArchivoReceptor['timestamp'] = pd.to_datetime(ArchivoReceptor['Fecha'] + ' ' + ArchivoReceptor['Hora'])

# Eliminación de las columnas "Fecha" y "Hora", una vez que se tiene la columna de "timestamp" para cada archivo
ArchivoEmisor.drop(['Fecha', 'Hora'], axis=1, inplace=True)
ArchivoReceptor.drop(['Fecha', 'Hora'], axis=1, inplace=True)

# Se unifican los datos de emisor y receptor por "ID". El ID 1 del emisor con el ID 1 del receptor
UnificacionIDs = pd.merge(ArchivoEmisor, ArchivoReceptor, on='ID', suffixes=('_emisor', '_receptor'))

# Calcular la latencia como la diferencia entre los timestamps
UnificacionIDs['latencia'] = (UnificacionIDs['timestamp_receptor'] - UnificacionIDs['timestamp_emisor']).dt.total_seconds() * 1000  # Se convierte en milisegundos

# Se extraen solo las columnas de "ID" y "latencia"
ResultadodeLatencia = UnificacionIDs[['ID', 'latencia']]

# Se muestra el resultado en shell
print(ResultadodeLatencia)

# Se guarda el resultado en un nuevo archivo CSV
ResultadodeLatencia.to_csv('Latencia-TLS.csv', index=False)


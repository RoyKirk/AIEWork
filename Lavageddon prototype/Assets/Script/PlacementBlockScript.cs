using UnityEngine;
using System.Collections;

public class PlacementBlockScript : MonoBehaviour {

    public bool placeable = true;

    void OnTriggerExit(Collider other)
    {
        placeable = true;
    }

    void OnTriggerEnter(Collider other)
    {
        placeable = false;
    }
}

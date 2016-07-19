using UnityEngine;
using System.Collections;

public class PlatformScript : MonoBehaviour {

	// Update is called once per frame
	void LateUpdate ()
    {
        transform.eulerAngles = new Vector3(0, 0, 0);
        transform.localEulerAngles = new Vector3(0, 0, 0);
    }
}

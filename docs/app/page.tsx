import { buttonVariants } from "@/components/ui/button";
import { page_routes } from "@/lib/routes-config";
import Link from "next/link";

export const metadata = {
  title: "Graphite - enabling seamless storage expansion via Bluetooth and companion app." , 
  description: "Our fourth-year capstone project, Which aims to extend mobile phone storage using an ESP32-powered Bluetooth hard drive and a cross platform companion mobile app .",
};

export default function Home() {
  return (
    <div className="flex sm:min-h-[91vh] min-h-[88vh] flex-col items-center justify-center text-center px-2 py-8">
      <h1 className="text-5xl font-bold mb-4 sm:text-7xl">
        Graphite  
      </h1>
      <h1 className="text-3xl font-bold mb-4 sm:text-5xl">
        Gone are the days of storage problems 
      </h1>
      <p className="mb-8 sm:text-md max-w-[800px] text-muted-foreground">
        enabling seamless storage expansion via Bluetooth and companion app.
      </p>
      <div>
        <Link
          href={`/docs${page_routes[0].href}`}
          className={buttonVariants({
            className: "px-6 !font-medium",
            size: "lg",
          })}
        >
          Learn More
        </Link>
      </div>
    </div>
  );
}

